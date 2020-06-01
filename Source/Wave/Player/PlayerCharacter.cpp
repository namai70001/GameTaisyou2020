// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#include "PlayerCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "../WaterSurface/WaterSurface.h"
#include "../WaterSurface/Raft.h"
#include "Kismet/GameplayStatics.h"
#include "../Object/GameController.h"
#include "../Camera/GameCameraActor.h"
#include "../InputManager.h"
#include "../UI/PauseUI.h"
#include "../UI/HammerCountBarUI.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.h"
#include "Niagara/Public/NiagaraFunctionLibrary.h"
#include "../GlobalGameInstance.h"
#include "../MyFunc.h"
//////////////////////////////////////////////////////////////////////////
// APlayerCharacter

APlayerCharacter::APlayerCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Configure character movement
	//GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	//GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	//GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
	
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
	//�|�[�Y���ł�Tick������悤�ɂ���
	SetTickableWhenPaused(true);
}

//void APlayerCharacter::BeginPlay()
//{
//
//
//}

void APlayerCharacter::BeginPlay_C()
{
	// �V�[����̃Q�[���J��������������
	AGameCameraActor* cameraActor;
	cameraActor = Cast<AGameCameraActor>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameCameraActor::StaticClass()));
	if (cameraActor)
	{
		// �݂��Ƀv���C���[�ƃJ�����̎Q�Ƃ��Z�b�g
		FollowCamera = cameraActor;
		cameraActor->SetFollowTarget(this);
	}

	//���݂�BegibPlay�̓��f���̓s���ケ����ŏ����Ȃ��̂Ŋ֐��ŌĂ�
	IsAttackHold = false;
	IsPlayAttackAnime = false;
	HammerPower = 0.0f;
	HammerHP = MaxHammerHP;
	if (!AnimInst)
	{
		AnimInst = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
		// �n���}�[�Œ@�������ɌĂ΂��֐���o�^
		AnimInst->AttackEndCallBack.BindUObject(this, &APlayerCharacter::HummerAttackEnd);
	}
	CreateHammerCountBarUI();

	PrevPos = FVector::ZeroVector;

	Water = Cast<AWaterSurface>(UGameplayStatics::GetActorOfClass(GetWorld(), AWaterSurface::StaticClass()));

	IsRaftRiding = false;

	IsRide = true;
}

void APlayerCharacter::Tick(float DeltaTime)
{
	//�A�^�b�N�A�j�����Đ������m�F
	IsPlayAttackAnime = AnimInst->GetIsAttackAnime();
	//�A�^�b�N�A�j�����̓|�[�Y���J���Ȃ��悤�ɂ���
	if (!IsPlayAttackAnime && !IsAttackHold)
	{
		PauseInput();
		if (UGameplayStatics::IsGamePaused(GetWorld()))
		{//�|�[�Y���̓|�[�Y�̓��͂����󂯕t���Ȃ�
			return;
		}
	}

	const AInputManager * inputManager = AInputManager::GetInstance();
	if (inputManager)
	{
		const InputState * input = inputManager->GetState();
		if (input->Attack.IsPress) TriggerHammerAttack();
		else if (input->Attack.IsRelease) ReleaseHammerAttack();

		float MoveSpeed = 10.0f;
		if (AnimInst->GetIsCharge()) MoveSpeed *= 0.5f;

		// ���X�e�B�b�N�̓|����̊������Z�o
		MoveAmount = FMath::Clamp(FMath::Abs(input->LeftStick.Horizontal) + FMath::Abs(input->LeftStick.Vertical), 0.0f, 1.0f);
		
		// �J��������ړ�����������Z�o
		FVector Direction;
		if (FollowCamera != NULL)
		{
			FVector V_Direction = FollowCamera->GetActorForwardVector();
			if (FMath::Abs(V_Direction.Z) > 0.9f) { V_Direction = FollowCamera->GetActorUpVector(); } // �J�������^��ɂ��鎞�ɂ��Ή�
			V_Direction.Z = 0.0f; 
			V_Direction.Normalize();

			FVector H_Direction = FollowCamera->GetActorRightVector();
			H_Direction.Z = 0.0f; 
			H_Direction.Normalize();

			Direction = V_Direction * input->LeftStick.Vertical + H_Direction * input->LeftStick.Horizontal;
			Direction.Normalize();
		}

		// �i�ޕ����Ɍ�����ς���
		if (MoveAmount > 0.01f)
		{
			float angle = FMath::Atan2(Direction.Y, Direction.X);
			FQuat LookAt = MyFunc::FromAxisAngleToQuaternion(FVector::UpVector, angle);
			SetActorRotation(FQuat::Slerp(GetActorQuat(), LookAt, 0.1f));
		}

		FVector movedPos = GetActorLocation();
		if (input->Select.IsPress && IsRide)
		{
			IsRide = false;

			if (!IsRaftRiding)
			{
				UE_LOG(LogTemp, Log, TEXT("Charenge Ride"));
				TArray<AActor*> FoundActors;
				UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARaft::StaticClass(), FoundActors);
				for (auto Actor : FoundActors)
				{
					ARaft* raft = Cast<ARaft>(Actor);
					if (!raft) continue;
					UE_LOG(LogTemp, Log, TEXT("Found Raft"));
					if (!raft->IsRide(movedPos)) continue;

					UE_LOG(LogTemp, Log, TEXT("Ride"));
					CurrentRaft = raft;
					IsRaftRiding = true;
					FVector RidePos = CurrentRaft->GetActorLocation();
					RidePos.Z = CurrentRaft->GetRiderZ();
					movedPos = RidePos;
					PrevPos = RidePos;
				}
			}
			else
			{
				FVector GetOffPos = CurrentRaft->GetGetOffPos();
				if (GetOffPos != FVector::ZeroVector)
				{
					UE_LOG(LogTemp, Log, TEXT("Get Off"));
					GetOffPos.Z = CurrentRaft->GetRiderZ();
					SetActorLocation(GetOffPos);
					movedPos = GetOffPos;
					IsRaftRiding = false;
				}
			}
		}

		if (input->Select.IsRelease)
		{
			IsRide = true;
		}

		// �C�J�_�ɏ���Ă��鎞
		if (IsRaftRiding)
		{
			movedPos = CurrentRaft->GetMoveVec() + movedPos;
			SetActorLocation(movedPos);
			if (!CurrentRaft->IsOnRaft(GetActorLocation()))
			{
				FVector moveVec = movedPos - PrevPos;
				SetActorLocation(CurrentRaft->AdjustMoveOnRaft(PrevPos, moveVec));
			}
			else
			{
				Move(Direction, MoveAmount * MoveSpeed);
			}
		}
		// �ʏ펞
		else
		{
			float WaterCheckRadius = Radius * 1.2f;
			float dist = WaterCheckRadius * 1.3f;
			FVector CurPos = GetActorLocation();
			FVector moveForce = Direction * MoveAmount * MoveSpeed;
			FVector moveDir = moveForce.GetSafeNormal();
			FVector WaterCheckPos = CurPos + moveDir * dist;

			UKismetSystemLibrary::DrawDebugCylinder(this, WaterCheckPos - FVector(0, 0, 50), WaterCheckPos - FVector(0, 0, 40), WaterCheckRadius, 64, FLinearColor::Blue, 0.0f, 3.0f);
			UKismetSystemLibrary::DrawDebugCylinder(this, CurPos + moveForce - FVector(0, 0, 50), CurPos + moveForce - FVector(0, 0, 40), Radius, 64, FLinearColor::Red, 0.0f, 3.0f);

			FVector NextPos = Water->AdjustMoveInLand(CurPos, moveForce, Radius, WaterCheckPos, WaterCheckRadius);
			
			float MoveValue = 0.0f;
			(NextPos - CurPos).ToDirectionAndLength(Direction, MoveValue);
			// �ŏI�I�Ɍ��肵���ړ��ʂ����Z
			Move(Direction, MoveValue);
		}
		PrevPos = movedPos;
	}

	if (IsAttackHold)
	{
		// ���߂���ő�l�ȏ�͂����Ȃ�
		if (HammerPower < ChargePowerMax)
		{
			//�n���}�[�𗭂߂Ă�����͂𑫂�
			HammerPower += ChargeSpeed;
			HammerPower = FMath::Min(HammerPower, ChargePowerMax);
			MinusHammerGauge(HammerPower);
		}
		
	}

	//�J�����Ƀ��C���΂��ē�����Ȃ���΃A�E�g���C���K�p
	ACharacter* myCharacter = this;
	FVector Start = this->GetActorLocation();
	FVector End = (FollowCamera->Camera->GetComponentLocation() - Start) * 10000 + Start;

	FHitResult HitData(ForceInit);
	if (Trace(AActor::GetWorld(), myCharacter, Start, End, HitData) && HitData.GetActor()) OutLineDraw();
	else OutLineNotDraw();
}

void APlayerCharacter::Move(const FVector & Direction, float Value)
{
	if (IsPlayAttackAnime)return;
	if (Value < 0.05f) return;

	//FVector NextPos = GetActorLocation() + Direction * Value;
	//SetActorLocation(NextPos);

	AddMovementInput(Direction, Value);
	// �������ړ��ʂ𒲐�
	float FallForce = GetMovementComponent()->Velocity.Z;
	GetMovementComponent()->Velocity = Direction * Value * 40.0f;
	GetMovementComponent()->Velocity.Z = FallForce;
}

void APlayerCharacter::TriggerHammerAttack(void)
{
	if (IsPlayAttackAnime)return;
	AnimInst = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	AnimInst->HummerChergeEvent();
	IsAttackHold = true;
	HammerCountBarUI->FirstEvent();
}

void APlayerCharacter::ReleaseHammerAttack(void)
{
	if (!IsAttackHold) return;
	AnimInst = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	AnimInst->HummerAttackEvent();
	IsAttackHold = false;
	HammerCountBarUI->ReflectionGauge();
}

void APlayerCharacter::HummerAttackEnd()
{
	if (AttackEffect && AttackEffect->IsValid() && HummerTipPoint)
	{
		// �n���}�[�̐�[���擾
		FVector AttackPoint = HummerTipPoint->GetComponentLocation();
		// �g���N����
		WaterAttack(AttackPoint, HammerPower * HammerPowerValue);
		// �G�t�F�N�g�𐶐�
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, AttackEffect, AttackPoint, GetActorRotation(), FVector::OneVector * AttackEffectScale, true);
		// �J�����V�F�C�N
		if(FollowCamera) FollowCamera->EventCameraShake(HammerPower);
	}
	HammerPower = 0.0f;
}

void APlayerCharacter::MinusHammerGauge(const float Power)
{
	if (!HammerCountBarUI)return;
	HammerHP -= ChargeSpeed;
	if (HammerHP < 0.0f)
	{
		HammerHP = 0.0f;
	}
	HammerCountBarUI->UpdateGauge(HammerHP);
}

void APlayerCharacter::PauseInput()
{
	const AInputManager * inputManager = AInputManager::GetInstance();
	if (!inputManager)return;
	const InputState * input = inputManager->GetState();
	if (input->Pause.IsPress)
	{//�|�[�Y���łȂ���΃|�[�Y��ʂ��J���A�|�[�Y����������|�[�Y��ʂ����
		if (!UGameplayStatics::IsGamePaused(GetWorld()))
		{
			if (PauseUIClass != nullptr)
			{//���߂ă|�[�Y��ʂ��J���Ƃ���UI�𐶐�����
				if (!PauseUI)
				{
					PauseUI = CreateWidget<UPauseUI>(GetWorld(), PauseUIClass);
					PauseUI->AddToViewport();
					//�|�[�Y�p�̃o�[���X�V���邽��HP��n��
					PauseUI->SetMaxHP(MaxHammerHP);
					PauseUI->SetHP(HammerHP);
				}
				else if (PauseUI)
				{
					if (PauseUI->GetIsPlayAnimation())return;
					PauseUI->AddToViewport();
					PauseUI->SetHP(HammerHP);
				}
				//�������Ă�nullptr��������G���[���\��
				if(PauseUI == nullptr)
				{
					UE_LOG(LogTemp, Error, TEXT("PauseUI : %s"), L"Widget cannot create");
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("PauseUI : %s"), L"PauseUIClass is nullptr");
			}
		}
		else if (UGameplayStatics::IsGamePaused(GetWorld()))
		{
			if (!PauseUI)return;
			if (PauseUI->GetIsPlayAnimation())return;
			PauseUI->EndPlayAnimation();
		}
	}
	if (!UGameplayStatics::IsGamePaused(GetWorld()))return;
	if (!PauseUI)return;
	if (input->Up.IsPress)
	{
		PauseUI->BackSelectState();
	}
	if (input->Down.IsPress)
	{
		PauseUI->NextSelectState();
	}
	if (input->Select.IsPress)
	{
		PauseUI->SelectStateAction();
	}
}

void APlayerCharacter::WaterAttack(FVector Point, float Power)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWaterSurface::StaticClass(), FoundActors);
	
	for (auto Actor : FoundActors)
	{
		AWaterSurface* water = Cast<AWaterSurface>(Actor);
		if (water)
		{
			water->AddPower(Point, Power * 100.0f);
		}
	}
}

void APlayerCharacter::CreateHammerCountBarUI()
{
	if (HammerCountBarUI)return;
	//BarUI����
	if (HammerCountBarUIClass != nullptr)
	{
		HammerCountBarUI = CreateWidget<UHammerCountBarUI>(GetWorld(), HammerCountBarUIClass);
		HammerCountBarUI->AddToViewport();
		HammerCountBarUI->SetMaxHammerHP(MaxHammerHP);
		//�������Ă�nullptr��������G���[���\��
		if (HammerCountBarUI == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("PauseUI : %s"), L"Widget cannot create");
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PauseUI : %s"), L"HammerCountBarUIClass is nullptr");
	}

}

void APlayerCharacter::SetNormaPercent(const float percent)
{
	if (HammerCountBarUI)
	{
		HammerCountBarUI->SetNormaPercent(percent);
	}
	else
	{
		CreateHammerCountBarUI();
		HammerCountBarUI->SetNormaPercent(percent);
	}
}

void APlayerCharacter::HammerCountBarParent()
{
	if (HammerCountBarUI)
	{
		HammerCountBarUI->RemoveFromParent();
	}
}