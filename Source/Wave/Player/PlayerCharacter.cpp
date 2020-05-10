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

	//コンポーネントを作成
	// Create a camera boom (pulls in towards the player if there is a collision)
	//CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	//CameraBoom->SetupAttachment(RootComponent);
	//CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	//CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	//// Create a follow camera
	//FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	//FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	//FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
	//ポーズ中でもTickが来るようにする
	SetTickableWhenPaused(true);
}

//void APlayerCharacter::BeginPlay()
//{
//
//
//}


void APlayerCharacter::BeginPlay_C()
{
	//現在のBegibPlayはモデルの都合上こちらで書けないので関数で呼ぶ
	IsAttackHold = false;

	IsPlayAttackAnime = false;

	HammerPower = 0.0f;
}

void APlayerCharacter::Tick(float DeltaTime)
{
	PauseInput();
	if (UGameplayStatics::IsGamePaused(GetWorld()))
	{//ポーズ中はポーズの入力しか受け付けない
		return;
	}
	if (!AnimInst)
	{
		AnimInst = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	}
	const AInputManager * inputManager = AInputManager::GetInstance();
	if (inputManager)
	{
		const InputState * input = inputManager->GetState();
		if (input->Attack.IsPress) TriggerHammerAttack();
		else if (input->Attack.IsRelease) ReleaseHammerAttack();

		MoveForward(input->LeftStick.Vertical);
		MoveRight(input->LeftStick.Horizontal);
	}

	//アタックアニメが再生中か確認
	IsPlayAttackAnime = AnimInst->GetIsAttackAnime();
	if (IsAttackHold)
	{//ハンマーを溜めていたら力を足す
		HammerPower += 0.1f;
	}

}


// Input

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
		if (FMath::Abs(Direction.Z) > 0.9f){ Direction = FollowCamera->GetActorUpVector(); } // カメラが真上にある時にも対応
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
	AnimInst = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	AnimInst->HummerAttackEvent();
	WaterAttack();
	IsAttackHold = false;
	HammerPower = 0.0f;

	// 攻撃カウント増加
	AttackCount++;

	MinusHammerCount();
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
	{//ポーズ中でなければポーズ画面を開き、ポーズ中だったらポーズ画面を閉じる
		if (!UGameplayStatics::IsGamePaused(GetWorld()))
		{
			if (UIClass != nullptr)
			{//初めてポーズ画面を開くときはUIを生成する
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
				//生成してもnullptrだったらエラー文表示
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

void APlayerCharacter::WaterAttack()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWaterSurface::StaticClass(), FoundActors);
	
	for (auto Actor : FoundActors)
	{
		AWaterSurface* water = Cast<AWaterSurface>(Actor);
		if (water)
		{
			water->AddPower(GetActorLocation(), HammerPower * 100.0f);
		}
	}
}
