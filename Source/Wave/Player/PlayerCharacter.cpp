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
#include "Kismet/GameplayStatics.h"
#include "../Object/Goal.h"
#include "../Camera/GameCameraActor.h"
#include "../InputManager.h"
#include "../UI/PauseUI.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.h"
#include "Niagara/Public/NiagaraFunctionLibrary.h"
//////////////////////////////////////////////////////////////////////////
// APlayerCharacter

APlayerCharacter::APlayerCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
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
	//���݂�BegibPlay�̓��f���̓s���ケ����ŏ����Ȃ��̂Ŋ֐��ŌĂ�
	IsAttackHold = false;
	IsPlayAttackAnime = false;
	HammerPower = 0.0f;

	if (!AnimInst)
	{
		AnimInst = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
		// �n���}�[�Œ@�������ɌĂ΂��֐���o�^
		AnimInst->AttackEndCallBack.BindUObject(this, &APlayerCharacter::HummerAttackEnd);
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	PauseInput();
	if (UGameplayStatics::IsGamePaused(GetWorld()))
	{//�|�[�Y���̓|�[�Y�̓��͂����󂯕t���Ȃ�
		return;
	}
	if (!AnimInst)
	{
		AnimInst = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	}
	//�A�^�b�N�A�j�����Đ������m�F
	IsPlayAttackAnime = AnimInst->GetIsAttackAnime();

	const AInputManager * inputManager = AInputManager::GetInstance();
	if (inputManager)
	{
		const InputState * input = inputManager->GetState();
		if (input->Attack.IsPress) TriggerHammerAttack();
		else if (input->Attack.IsRelease) ReleaseHammerAttack();

		MoveForward(input->LeftStick.Vertical);
		MoveRight(input->LeftStick.Horizontal);
	}

	if (IsAttackHold)
	{//�n���}�[�𗭂߂Ă�����͂𑫂�
		HammerPower += 0.1f;
	}

}

void APlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
}

void APlayerCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void APlayerCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void APlayerCharacter::MoveForward(float Value)
{
	if (IsAttackHold)return;
	if (IsPlayAttackAnime)return;

	if ((FollowCamera != NULL) && (Value != 0.0f))
	{
		FVector Direction = FollowCamera->GetActorForwardVector();
		if (FMath::Abs(Direction.Z) > 0.9f){ Direction = FollowCamera->GetActorUpVector(); } // �J�������^��ɂ��鎞�ɂ��Ή�
		Direction.Z = 0.0f; Direction.Normalize();
		AddMovementInput(Direction, Value);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if (IsAttackHold)return;
	if (IsPlayAttackAnime)return;

	if ((FollowCamera != NULL) && (Value != 0.0f))
	{
		FVector Direction = FollowCamera->GetActorRightVector();
		Direction.Z = 0.0f; Direction.Normalize();
		AddMovementInput(Direction, Value);
	}
}

void APlayerCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::TriggerHammerAttack(void)
{
	if (IsPlayAttackAnime)return;
	AnimInst = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	AnimInst->HummerChergeEvent();
	IsAttackHold = true;
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
		WaterAttack(AttackPoint, HammerPower);
		// �G�t�F�N�g�𐶐�
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, AttackEffect, AttackPoint, GetActorRotation(), FVector::OneVector * AttackEffectScale, true);
	}
	// �n���}�[�̒@����J�E���g�����炷
	MinusHammerCount();
	HammerPower = 0.0f;
}

void APlayerCharacter::MinusHammerCount()
{
	AGoal* goal;
	goal = Cast<AGoal>(UGameplayStatics::GetActorOfClass(GetWorld(), AGoal::StaticClass()));
	if (goal)
	{
		goal->MinusHammerCount();
	}
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
			if (UIClass != nullptr)
			{//���߂ă|�[�Y��ʂ��J���Ƃ���UI�𐶐�����
				if (!PauseUI)
				{
					PauseUI = CreateWidget<UPauseUI>(GetWorld(), UIClass);
					PauseUI->AddToViewport();
				}
				else if (PauseUI)
				{
					if (PauseUI->GetIsPlayAnimation())return;
					PauseUI->AddToViewport();
				}
				//�������Ă�nullptr��������G���[���\��
				if(PauseUI == nullptr)
				{
					UE_LOG(LogTemp, Error, TEXT("PauseUI : %s"), L"Widget cannot create");
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("PauseUI : %s"), L"UIClass is nullptr");
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
