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
#include "../SoundManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "../Camera/State/GameCameraStateFall.h"
#include "../Camera/State/GameCameraStateClear.h"
#include "../Camera/GameCameraFocusPoint.h"
#define ARRAY_HAMMER_POWER_MAX (10)
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

	//�|�[�Y���ł�Tick������悤�ɂ���
	SetTickableWhenPaused(true);
	//�n���}�[�p���[�z�񏉊���
	HammerPowerArray.Init(0.0f, ARRAY_HAMMER_POWER_MAX);
}

void APlayerCharacter::BeginPlay_C()
{
	// ���g�̃J�v�Z���R���W�����̑傫�����擾
	UCapsuleComponent * Collision = Cast<UCapsuleComponent>(GetComponentByClass(UCapsuleComponent::StaticClass()));
	if (Collision)
		this->CollisionRadius = Collision->GetScaledCapsuleRadius();

	AnimInst = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	// �n���}�[�Œ@�������ɌĂ΂��֐���o�^
	AnimInst->AttackEndCallBack.BindUObject(this, &APlayerCharacter::HummerAttackEnd);

	// �V�[����̃Q�[���J��������������
//	if (BattleNumber == 0)
	{
		FollowCamera = Cast<AGameCameraActor>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameCameraActor::StaticClass()));
	}
	
	if (FollowCamera)
	{
		// �݂��Ƀv���C���[�ƃJ�����̎Q�Ƃ��Z�b�g
		FollowCamera->SetFollowTarget(this);
	}
	HammerPower = 0.0f;
	HammerHP = 0.0f;

	//if (BattleNumber <= 1)
	{
		CreateHammerCountBarUI();
	}
	Water = Cast<AWaterSurface>(UGameplayStatics::GetActorOfClass(GetWorld(), AWaterSurface::StaticClass()));

	CurrentRaft = nullptr;
	IsDeth = false;

	if(BattleNumber == 2)
		InputManagerIndex = EAutoReceiveInput::Player1;
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (NoTick)return;
	if (UGameplayStatics::IsGamePaused(GetWorld()))
	{//�|�[�Y���̓��^�[��	
		return;
	}
	// ���S���͉����ł��Ȃ�
	if (IsDeth) return;
	if (AnimInst->IsClear) return;

	FVector CurPos = GetActorLocation();

	// �X�V�̎n�߂Ƀv���C���[�̉����m�F����
	if (!CheckGround())
	{
		// �������s������v���C���[�͗�����������ɂ���
		Water->SetCollisionEnabled(false);	// WaterSurface�̃R���W�������I�t�ɐݒ�
		ChageDestroyEmmiter();

		if (CurPos.Z < -10.0f) // �v���C���[�̍��W�����ȉ��ɍs�������͎��S
		{
			if (Water->IsInWater(CurPos))
			{
				ASoundManager::SafePlaySound(SOUND_TYPE::FALL_PLAYER);	// ���̏�ɂ��鎞�����炷
			}
			else
			{
				ASoundManager::SafePlaySound(SOUND_TYPE::FALL_ACTOR);
			}

			FollowCamera->ChangeState(new GameCameraStateFall(CurPos));
			AGameCameraFocusPoint::SpawnFocusPoint(this, CurPos, 1.0f);

			UCapsuleComponent * Collision = Cast<UCapsuleComponent>(GetComponentByClass(UCapsuleComponent::StaticClass()));
			if (Collision)
			{
				Collision->ComponentVelocity = FVector::ZeroVector;
				Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				Collision->SetActive(false);
			}
			USkeletalMeshComponent * SkeletalMesh = Cast<USkeletalMeshComponent>(GetComponentByClass(USkeletalMeshComponent::StaticClass()));
			if (SkeletalMesh)
			{
				SkeletalMesh->ComponentVelocity = FVector::ZeroVector;
			}

			AnimInst->IsDeth = true;
			PlayerDeth();
			Water->AddPower(FVector(CurPos.X, CurPos.Y, 0.0f), ChargePowerMax);
			IsDeth = true;
			return;
		}
	}

	const AInputManager * inputManager = AInputManager::GetInstance(InputManagerIndex);
	if (inputManager)
	{
		const InputState * input = inputManager->GetState();
		if (input->Attack.IsPress) TriggerHammerAttack();
		else if (input->Attack.IsRelease) ReleaseHammerAttack();

		float MoveSpeed = 10.0f;
		if (AnimInst->IsCharge) MoveSpeed *= 0.3f;

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

		// �`���[�W��
		//if (BattleNumber <= 1)
		{
			UpdateGaugeHP();
		}
		if (AnimInst->IsChargeAnim())
		{
			// �L�����Z���������ꂽ��
			if (input->AttackCancel.IsPress)
			{
				AnimInst->IsCharge = false;
				HammerHP = 0;
				HammerPower = 0.0f;
				ChargeCount = 0.0f;
				//if (BattleNumber <= 1)
				{
					HammerCountBarUI->UpdateGauge(HammerHP);
				}
			}
			else
			{
				ChageCreateEmmiter();
				// ���߂���ő�l�ȏ�͂����Ȃ�
				if (HammerPower < ChargePowerMax)
				{
					//�n���}�[�𗭂߂Ă�����͂𑫂�
					//�̗͂��ق�0�Ȃ痭�߂Ă����Z���Ȃ�
					if (HammerHP < MaxHammerHP - 0.1f)
					{
						if (!AudioComponent)AudioComponent = ASoundManager::CreateAudioComponent(SOUND_TYPE::HAMMER_CHARGE);
						if (!AudioComponent->IsPlaying())AudioComponent->Play();
						HammerPower += ChargeSpeed;
						HammerPower = FMath::Min(HammerPower, ChargePowerMax);
					}
					MinusHammerGauge(HammerPower);
				}
				else
				{//�ő嗭�ߎ��̓Q�[�W��_�ł�����
					if (HammerCountBarUI)
					{
						HammerCountBarUI->PlayGaugeAnimation();
					}
				}
			}
		}
		else
		{
			if (AudioComponent && !(AudioComponent->IsPlaying()))
			{
				AudioComponent->Stop();
			}

			ChageDestroyEmmiter();
		}
	}// !InputManager	

	ChageUpDateEmmiter();

	//�J�����Ƀ��C���΂��ē�����Ȃ���΃A�E�g���C���K�p
	if (FollowCamera)
	{
		ACharacter* myCharacter = this;
		FVector Start = this->GetActorLocation();
		FVector End = (FollowCamera->Camera->GetComponentLocation() - Start) * 10000 + Start;

		FHitResult HitData(ForceInit);
		if (Trace(AActor::GetWorld(), myCharacter, Start, End, HitData) && HitData.GetActor()) OutLineDraw();
		else OutLineNotDraw();
	}
}

void APlayerCharacter::Move(const FVector & Direction, float Value)
{
	if (AnimInst->IsAttack)return;
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
	if (AnimInst->IsAttack || AnimInst->IsCharge)return;
	if (HammerHP >= MaxHammerHP)return;
	
	AnimInst->IsCharge = true;
	if (!HammerCountBarUI)return;
	HammerCountBarUI->FirstEvent();
}

void APlayerCharacter::ReleaseHammerAttack(void)
{
	if (!AnimInst->IsCharge) return;
	AnimInst->IsAttack = true;
	AnimInst->IsCharge = false;
}

void APlayerCharacter::HummerAttackEnd()
{
	if (AttackEffect && AttackEffect->IsValid() && HummerTipPoint)
	{
		// �n���}�[�̐�[���擾
		FVector AttackPoint = HummerTipPoint->GetComponentLocation();
		// �g���N����
		//WaterAttack(AttackPoint, HammerPower * HammerPowerValue, 15.0f);
		WaterAttack(AttackPoint, GetArrayHammerPower(), 15.0f);
		// �G�t�F�N�g�𐶐�
		ImpactEmmiterCreate(AttackPoint);
		//UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, AttackEffect, AttackPoint, GetActorRotation(), FVector::OneVector * AttackEffectScale, true);
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
	if (!HammerCountBarUI)return;
	//�ő嗭�ߏ�Ԃ�������_�ŃA�j�����~
	if (HammerPower >= ChargePowerMax)
	{
		HammerCountBarUI->PouseGaugeAnimation();
	}
	HammerCountBarUI->ReflectionGauge();
	HammerPower = 0.0f;
	ChargeCount = 0.0f;
	HammerHP = 0;
	HammerCountBarUI->UpdateGauge(HammerHP);
}

void APlayerCharacter::MinusHammerGauge(const float Power)
{
	if (!HammerCountBarUI)return;
	if (ChargeCount < ChargePowerMax)
	{//�ő嗭�ߎ��̓N�[���^�C�������Z���Ȃ�
		if (HammerHP < MaxHammerHP)
		{
			HammerHP += ChargeSpeed;
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
	if (HammerCountBarUI)
	{
		//if (BattleNumber <= 1)
		{
			HammerCountBarUI->UpdateGauge(HammerHP);
		}
	}
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

		if (CurrentRaft && !Water->GetLandPoint(Pos))
		{
			Water->SetCollisionEnabled(false);	// �����̏�ɏ���Ă�ꍇ��WaterSurface�R���W�������I�t�ɂ��Ă���
			if (CurrentRaft->GetIsFall())
				return false;
		}
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

float APlayerCharacter::GetArrayHammerPower()
{
	//�܂����݂̗��߂Ă���͂ƍő�p���[������~10�����������������
	float power = (HammerHP / ChargePowerMax) * 10.0f;
	//1�ɖ����Ȃ��ꍇ�͔z��0�Ԗڂ�Ԃ�
	if (power < 1.0f)
	{
		return HammerPowerArray[0];
	}
	//�z��̍ő吔�𒴂���͂������Ă�����z��̍ő�l��Ԃ�
	else if (power >= ARRAY_HAMMER_POWER_MAX)
	{
		return HammerPowerArray[ARRAY_HAMMER_POWER_MAX - 1];	
	}
	//����������؂�̂Đ����ɕϊ�
	int index = FMath::TruncToInt(power);
	return HammerPowerArray[index];
}

void APlayerCharacter::WaterAttack(FVector Point, float Power, float Radius)
{
	if (!Water) return;

	Water->AddPower(Point, Power * 100.0f);
	Water->HammerBreakLand(Point, Radius);

	if (Water->IsInLand(Point))
	{
		if (Power == ChargePowerMax)
		{
			ASoundManager::SafePlaySound(SOUND_TYPE::HAMMER_BIG);
		}
		else if (Power > ChargePowerMax * 0.5f)
		{
			ASoundManager::SafePlaySound(SOUND_TYPE::HAMMER_MEDIUM);
		}
		else
		{
			ASoundManager::SafePlaySound(SOUND_TYPE::HAMMER_SMALL);
		}
	}
	else
	{
		if (Power == ChargePowerMax)
		{
			ASoundManager::SafePlaySound(SOUND_TYPE::HAMMER_BIG_W);
		}
		else if (Power > ChargePowerMax * 0.5f)
		{
			ASoundManager::SafePlaySound(SOUND_TYPE::HAMMER_MEDIUM_W);
		}
		else
		{
			ASoundManager::SafePlaySound(SOUND_TYPE::HAMMER_SMALL_W);
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
			UE_LOG(LogTemp, Error, TEXT("HammerCountBarUI : %s"), L"Widget cannot create");
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HammerCountBarUI : %s"), L"HammerCountBarUIClass is nullptr");
	}

}

void APlayerCharacter::HammerCountBarParent()
{
	if (HammerCountBarUI)
	{
		HammerCountBarUI->RemoveFromParent();
		HammerCountBarUI = nullptr;
	}
}

void APlayerCharacter::DebugHammerCountBarParent()
{
	if (HammerCountBarUI)
	{
		HammerCountBarUI->RemoveFromParent();
	}
	if (!HammerCountBarUI)
	{
		CreateHammerCountBarUI();
		HammerCountBarUI->RemoveFromParent();
	}
}

void APlayerCharacter::SetNoTick()
{
	this->SetActorTickEnabled(false);
	NoTick = true;
	MoveAmount = 0.0f;
}

void APlayerCharacter::SetGameFinish(bool isClear)
{
	((isClear) ? AnimInst->IsClear : AnimInst->IsGameOver) = true;

	FollowCamera->ChangeState(new GameCameraStateClear());

	// �v���C���[���C�J�_�ɏ���Ă�����C�J�_�̍X�V���~�߂�
	if (CurrentRaft != nullptr)
	{
		CurrentRaft->StopRaftAudio();
		CurrentRaft->SetActorTickEnabled(false);
	}
	ChageDestroyEmmiter();
}

void APlayerCharacter::SetPlayerHiddenInGame()
{
	this->SetActorHiddenInGame(true);
}

bool APlayerCharacter::GetIsAttack() const
{
	return AnimInst->IsAttack;
}

bool APlayerCharacter::GetIsCharge() const
{
	return AnimInst->IsCharge;
}

AGameCameraActor * APlayerCharacter::GetCameraActor()
{
	return FollowCamera;
}

void APlayerCharacter::UpdateGaugeHP()
{
	if (AnimInst->IsCharge || AnimInst->IsAttack)return;
	if (HammerPower > 0.0f)return;
	if (HammerCountBarUI)
	{
		if (!HammerCountBarUI->GetIsDamage())
		{
			HammerHP -= HpHealSpped;
			if (HammerHP < 0)
			{
				HammerHP = 0;
			}
			HammerCountBarUI->UpdateGauge(HammerHP);
			HammerCountBarUI->UpdateDamageGauge(HammerHP);
		}
		else
		{
			HammerCountBarUI->UpdateCoolTime(CoolTimeHealSpped);
		}
	}
}

void APlayerCharacter::PlayerRespawn(const FVector & location)
{
	SetActorLocation(location);
	IsDeth = false;
}