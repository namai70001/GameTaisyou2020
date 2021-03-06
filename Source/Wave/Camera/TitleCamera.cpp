// Fill out your copyright notice in the Description page of Project Settings.


#include "TitleCamera.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "../Object/TitleMemo.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
#include "../GlobalGameInstance.h"
// Sets default values
ATitleCamera::ATitleCamera()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// カメラブームを生成
// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

}

// Called when the game starts or when spawned
void ATitleCamera::BeginPlay()
{
	Super::BeginPlay();
	MyStageNumber = DefaultStageNumber;
	UGlobalGameInstance* instance = UGlobalGameInstance::GetInstance();
	if (instance)
	{
		if (instance->GetStageNumber() > -1)
		{
			MyStageNumber = instance->GetStageNumber();
		}
	}
}

// Called every frame
void ATitleCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	GetAllTitleMemo();
	//戻るボタンを押したらカメラを引く
	if (IsBack)
	{
		CameraBoom->TargetArmLength += 0.5f;
	}
}


void ATitleCamera::GetAllTitleMemo()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATitleMemo::StaticClass(), FoundActors);	
	MemoNum = FoundActors.Num();
	for (auto Actor : FoundActors)
	{
		ATitleMemo* memo = Cast<ATitleMemo>(Actor);
		if (memo)
		{
			if (MyStageNumber == memo->GetStageNumber())
			{
				TitleMemo = memo;
				this->SetActorLocation(FMath::Lerp(this->GetActorLocation(), memo->GetActorLocation(), CameraSpeed));
			}
		}
	}
}

void ATitleCamera::SelectInput()
{
	if (TitleMemo)
	{
		TitleMemo->FadeStart();
		IsSelectMap = true;
	}
}

void ATitleCamera::RightInput()
{
	MyStageNumber++;
	if (MyStageNumber == MemoNum)
	{
		MyStageNumber = 0;
	}
}

void ATitleCamera::LeftInput()
{
	MyStageNumber--;
	if (MyStageNumber < 0)
	{
		MyStageNumber = MemoNum - 1;
	}
}


void ATitleCamera::SetCenterTitleMemo()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATitleMemo::StaticClass(), FoundActors);
	MemoNum = FoundActors.Num();
	for (auto Actor : FoundActors)
	{
		ATitleMemo* memo = Cast<ATitleMemo>(Actor);
		if (memo)
		{
			if (MyStageNumber == memo->GetStageNumber())
			{
				TitleMemo = memo;
				this->SetActorLocation(memo->GetActorLocation());
			}
		}
	}
}