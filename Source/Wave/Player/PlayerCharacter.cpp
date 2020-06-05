// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#include "PlayerCharacter.h"
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
#include "EngineGlobals.h"
#include "Components/CapsuleComponent.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "GameFramework/PlayerController.h"

#define DISPLAY_LOG(fmt, ...) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT(fmt), __VA_ARGS__));
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

	OldAttackFrame = false;
	FreasTime = 0;
}

void APlayerCharacter::BeginPlay_C()
{
	// ���g�̃J�v�Z���R���W�����̑傫�����擾
	UCapsuleComponent * Collision = Cast<UCapsuleComponent>(GetComponentByClass(UCapsuleComponent::StaticClass()));
	if (Collision)
		this->CollisionRadius = Collision->GetScaledCapsuleRadius();

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

	Water = Cast<AWaterSurface>(UGameplayStatics::GetActorOfClass(GetWorld(), AWaterSurface::StaticClass()));

	CurrentRaft = nullptr;
	IsDeth = false;
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
	// ���S���͉����ł��Ȃ�
	if (IsDeth) return;

	FVector CurPos = GetActorLocation();

	// �X�V�̎n�߂Ƀv���C���[�̉����m�F����
	if (CheckGround())
	{
		
	}
	// �������s������v���C���[�͗�����������ɂ���
	else
	{
		Water->SetCollisionEnabled(false);	// WaterSurface�̃R���W�������I�t�ɐݒ�

		if (CurPos.Z < -10.0f) // �v���C���[�̍��W�����ȉ��ɍs�������͎��S
		{
			Water->AddPower(FVector(CurPos.X, CurPos.Y, 0.0f), ChargePowerMax);
			IsDeth = true;
			PlayerDeth();
			UCapsuleComponent * Collision = Cast<UCapsuleComponent>(GetComponentByClass(UCapsuleComponent::StaticClass()));
			if (Collision)Collision->SetActive(false);
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
		if (AnimInst->GetIsCharge()) MoveSpeed *= 0.3f;

		// ���X�e�B�b�N�̓|����̊������Z�o
		MoveAmount = FMath::Clamp(FMath::Abs(input->LeftStick.Horizontal) + FMath::Abs(input->LeftStick.Vertical), 0.0f, 1.0f);
		// �J��������ړ�����������Z�o
		FVector Direction = GetInputDirection(input->LeftStick.Vertical, input->LeftStick.Horizontal);
		// �i�ޕ����Ɍ�����ς���
		if (MoveAmount > 0.01f)
			SetLookAt(Direction, 0.15f);

		FVector moveForce = Direction * MoveAmount * MoveSpeed;
		FVector moveDir = moveForce.GetSafeNormal();

		// �C�J�_�ɏ���Ă��鎞
		if (CurrentRaft != nullptr)
		{
			FHitResult HitResult(ForceInit);
			
			if (!IsInRaft)
			{
				if (CurrentRaft->IsInRaft(CurPos, CollisionRadius))
				{
					IsInRaft = true;
				}
				// �C�J�_�̏�ɂ��邪�͂ݏo�Ă�����
				else
				{
					// �����I�ɃC�J�_�̏�Ɉړ�����
					FVector SubDirection = CurrentRaft->GetActorLocation() - CurPos;
					MoveAmount = 1.0f;
					SetLookAt(SubDirection, 0.5f);
					Move(SubDirection.GetSafeNormal2D(), MoveSpeed * 0.5f);
				}
			}
			else if (Water->GetLandPoint(CurPos + Direction * CollisionRadius * 2.0f, CollisionRadius * 0.5f) ||			// ������������
					CheckTraceGround(HitResult, CurPos + Direction * CollisionRadius * 2.0f, CollisionRadius * 0.3f, CurrentRaft))   // ���̃C�J�_����������
			{
				Move(Direction, MoveSpeed * MoveAmount);
			}
			// �C�J�_�̏�Ɋ��S�ɏ���Ă�����
			else
			{
				FVector NextPos = CurrentRaft->AdjustMoveOnRaft(CurPos, moveForce, CollisionRadius);
				float MoveValue = 0.0f;
				(NextPos - CurPos).ToDirectionAndLength(Direction, MoveValue);
				Move(Direction, MoveValue);
			}
		}
		// �ʏ펞
		else
		{
			// �C�J�_����������
			FHitResult HitResult(ForceInit);
			if (CheckTraceGround(HitResult, CurPos + Direction * CollisionRadius * 2.0f, CollisionRadius * 0.5f))
			{
				Move(Direction, MoveSpeed * MoveAmount);
			}
			// �n��ɂ��鎞
			else
			{
				float WaterCheckRadius = CollisionRadius * 1.2f;
				float dist = WaterCheckRadius * 1.3f;
				FVector WaterCheckPos = CurPos + moveDir * dist;

				// �f�o�b�O�`��
				UKismetSystemLibrary::DrawDebugCylinder(this, WaterCheckPos - FVector(0, 0, 50), WaterCheckPos - FVector(0, 0, 40), WaterCheckRadius, 64, FLinearColor::Blue, 0.0f, 3.0f);
				UKismetSystemLibrary::DrawDebugCylinder(this, CurPos + moveForce - FVector(0, 0, 50), CurPos + moveForce - FVector(0, 0, 40), CollisionRadius, 64, FLinearColor::Red, 0.0f, 3.0f);

				FVector NextPos = Water->AdjustMoveInLand(CurPos, moveForce, CollisionRadius, WaterCheckPos, WaterCheckRadius);
				float MoveValue = 0.0f;
				(NextPos - CurPos).ToDirectionAndLength(Direction, MoveValue);
				// �ŏI�I�Ɍ��肵���ړ��ʂ����Z
				Move(Direction, MoveValue);
			}
		}
	}

	// �`���[�W��
	UpdateGaugeHP();
	if (IsAttackHold)
	{
		ChageCreateEmmiter();
		// ���߂���ő�l�ȏ�͂����Ȃ�
		if (HammerPower < ChargePowerMax)
		{
			//�n���}�[�𗭂߂Ă�����͂𑫂�
			//�̗͂��ق�0�Ȃ痭�߂Ă����Z���Ȃ�
			if (HammerHP > 0.1f)
			{
				HammerPower += ChargeSpeed;
				HammerPower = FMath::Min(HammerPower, ChargePowerMax);
			}
			MinusHammerGauge(HammerPower);
		}
		else
		{//�ő嗭�ߎ��̓Q�[�W��_�ł�����
			HammerCountBarUI->PlayGaugeAnimation();
		}
		
	}
	else
	{
		ChageDestroyEmmiter();
	}


	
	if (!OldAttackFrame && IsPlayAttackAnime)
	{
		ImpactEmmiterCreate(FreasTime);
		FreasTime = 0;
	}
	OldAttackFrame = IsPlayAttackAnime;
	if (IsAttackHold) FreasTime++;
	

	ChageUpDateEmmiter(CurPos);



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

	AddMovementInput(Direction, Value);
	// �������ړ��ʂ𒲐�
	UPawnMovementComponent * Movement = GetMovementComponent();
	float FallForce = Movement->Velocity.Z;
	float MoveScale = Value * 40.0f;
	// �ړ��ł���ʂ������Ă����璲��
	if (Movement->Velocity.SizeSquared2D() > MoveScale)
	{
		Movement->Velocity = Direction * MoveScale;
		Movement->Velocity.Z = FallForce;
	}
}

bool APlayerCharacter::CheckTraceGround(FHitResult & Result, const FVector & Start, float SphereRadius, AActor* Ignore)
{
	FVector End = Start - FVector::UpVector * 10000.0f;

	FCollisionQueryParams TraceParms(FName(TEXT("Player Check Ground")));
	TraceParms.bTraceComplex = false;			// ���G�ȏՓ˂�ǐՂ���K�v�����邩�ǂ���
	TraceParms.bReturnPhysicalMaterial = false; // ���ʂɕ����I�Ȏ������܂߂邩�ǂ����B
	TraceParms.AddIgnoredActor(this);			// �����͖�������
	if(Ignore) TraceParms.AddIgnoredActor(Ignore);
	ECollisionChannel CollisionChannel = ECC_Vehicle;	// ��蕨�ݒ肵���R���W�����������肷��
	FCollisionShape SphereCast = FCollisionShape::MakeSphere(SphereRadius); // �v���C���[�̑傫���̋��Ŕ��肷��
	if (GetWorld()->SweepSingleByObjectType(Result, Start, End, FQuat::Identity, CollisionChannel, SphereCast, TraceParms))
		return true;

	return false;
}

void APlayerCharacter::ResetRaftParam()
{
	CurrentRaft = nullptr;
	IsInRaft = false;
}

void APlayerCharacter::TriggerHammerAttack(void)
{
	if (IsPlayAttackAnime)return;
	//�N�[���^�C��������Ƃ��ƍő�HP��HP�������Ȃ�\����Ȃ�
	//if (MaxHammerHP == HammerHP)return;
	if (HammerHP <= 0.0f)return;
	if (CoolTime != 0.0f)return;
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
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
		if (PlayerController)
		{//�R���g���[���[�U���@����1:����(0.0-1.0)����2:���� �c��̈���:�R���g���[���[�̂ǂ̕�����U�������邩�H�i�S��true�őS�̂�U��)
			//�U���̋��������߂�
			float power = ChargeCount / ChargePowerMax;
			PlayerController->PlayDynamicForceFeedback(power, 0.35f, true, true, true, true);
		}	
	}
	//�ő嗭�ߏ�Ԃ�������_�ŃA�j�����~
	if (HammerPower >= ChargePowerMax)
	{
		HammerCountBarUI->PouseGaugeAnimation();
	}
	HammerPower = 0.0f;
	ChargeCount = 0.0f;
}

void APlayerCharacter::MinusHammerGauge(const float Power)
{
	if (!HammerCountBarUI)return;
	if (ChargeCount < ChargePowerMax)
	{//�ő嗭�ߎ��̓N�[���^�C�������Z���Ȃ�
		//�X�Ɍ��݂�HP��MAXHP��艺����������Z
		//if (MaxHammerHP > HammerHP)
		if (HammerHP > 0.0f)
		{
			CoolTime += ChargeSpeed;
			if (CoolTime >= ChargePowerMax)
			{
				CoolTime = ChargePowerMax;
			}
			HammerHP -= ChargeSpeed;
			if (HammerHP >= MaxHammerHP)
			{
				HammerHP = MaxHammerHP;
			}
			ChargeCount += ChargeSpeed;
			if (ChargeCount >= ChargePowerMax)
			{
				ChargeCount = ChargePowerMax;
			}
		}
	}

	HammerCountBarUI->UpdateGauge(HammerHP);
	HammerCountBarUI->UpdateCoolTime(CoolTime);
}

bool APlayerCharacter::CheckGround()
{
	FVector Pos = GetActorLocation();
	if (!Water->IsInField(Pos)) return false;	// �t�B�[���h�O�ɏo�Ă�false

	FHitResult HitData(ForceInit);
	if (CheckTraceGround(HitData, Pos, CollisionRadius * 0.8f))
	{
		ARaft* HitRaft = Cast<ARaft>(HitData.GetActor());
		// �C�J�_�ɏ������ԂňႤ�C�J�_�ɏ������
		if (HitRaft && HitRaft != CurrentRaft)
			ResetRaftParam();
		// �C�J�_�Ȃ�o�^���Ă���
		CurrentRaft = HitRaft;

		if(CurrentRaft && !Water->GetLandPoint(Pos))
			Water->SetCollisionEnabled(false);	// �����̏�ɏ���Ă�ꍇ��WaterSurface�R���W�������I�t�ɂ��Ă���

		//DISPLAY_LOG("HitGroundActor : %s", HitData.GetActor()->GetName().GetCharArray().GetData());
		return true;
	}
	// �C�J�_����~�肽���_�ŃC�J�_�Ŏg���ϐ������Z�b�g
	ResetRaftParam();

	/// �n�ʂ̏�ɑ��݂��Ă��Ȃ�������WaterSurface�R���W�������I�t�ɂ��Ă���
	Water->SetCollisionEnabled(true);

	// ����������Ȃ炻�̎��_��true
	if (Water->GetLandPoint(Pos) != nullptr)
		return true;

	return false;
}

FVector APlayerCharacter::GetInputDirection(float VerticalValue, float HolizontalValue)
{
	if (FollowCamera == NULL) return FVector::ZeroVector;

	FVector Result;

	FVector V_Direction = FollowCamera->GetActorForwardVector();
	if (FMath::Abs(V_Direction.Z) > 0.9f) { V_Direction = FollowCamera->GetActorUpVector(); } // �J�������^��ɂ��鎞�ɂ��Ή�
	V_Direction.Z = 0.0f;
	V_Direction.Normalize();

	FVector H_Direction = FollowCamera->GetActorRightVector();
	H_Direction.Z = 0.0f;
	H_Direction.Normalize();

	Result = V_Direction * VerticalValue + H_Direction * HolizontalValue;
	Result.Normalize();

	return Result;
}

void APlayerCharacter::SetLookAt(FVector Direction, float Speed)
{
	float angle = FMath::Atan2(Direction.Y, Direction.X);
	FQuat LookAt = MyFunc::FromAxisAngleToQuaternion(FVector::UpVector, angle);
	SetActorRotation(FQuat::Slerp(GetActorQuat(), LookAt, Speed));
}

void APlayerCharacter::PauseInput()
{
	const AInputManager * inputManager = AInputManager::GetInstance();
	if (!inputManager)return;
	const InputState * input = inputManager->GetState();
	if (input->Pause.IsPress)
	{//�|�[�Y���łȂ���΃|�[�Y��ʂ��J���A�|�[�Y����������|�[�Y��ʂ����
		AGameController* game = Cast<AGameController>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameController::StaticClass()));
		//�Q�[���I�������𖞂����Ă�����|�[�Y���J���Ȃ��悤�ɂ���
		if (game->GetIsClear() || game->GetIsGameOver())return;
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
					if (game)
					{
						game->SetTimeCountPause();
					}
				}
				else if (PauseUI)
				{
				if (PauseUI->GetIsPlayAnimation())return;
				PauseUI->AddToViewport();
				PauseUI->SetHP(HammerHP);
				if (game)
				{
					game->SetTimeCountPause();
				}
				}
				//�������Ă�nullptr��������G���[���\��
				if (PauseUI == nullptr)
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
			if (game)
			{
				game->SetTimeCountRePlay();
			}
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
		HammerCountBarUI->SetMaxChargePowerMax(ChargePowerMax);
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

void APlayerCharacter::HammerCountBarParent()
{
	if (HammerCountBarUI)
	{
		HammerCountBarUI->RemoveFromParent();
	}
}

void APlayerCharacter::SetNoTick()
{
	this->SetActorTickEnabled(false);
	MoveAmount = 0.0f;
}

void APlayerCharacter::SetPlayerHiddenInGame()
{
	this->SetActorHiddenInGame(true);
}

void APlayerCharacter::UpdateGaugeHP()
{
	if (IsPlayAttackAnime)return;
	if (IsAttackHold)return;
	if (HammerPower > 0.0f)return;
	CoolTime -= ChargeSpeed * CoolTimeHealSpped;

	if (CoolTime < 0.0f)
	{
		CoolTime = 0.0f;
	}
	if (HammerCountBarUI)
	{
		if (CoolTime <= 0.0f)
		{
			HammerHP += ChargeSpeed * HpHealSpped;
			if (HammerHP > MaxHammerHP)
			{
				HammerHP = MaxHammerHP;
			}
			HammerCountBarUI->UpdateGauge(HammerHP);
		}
		HammerCountBarUI->UpdateCoolTime(CoolTime);
	}
}