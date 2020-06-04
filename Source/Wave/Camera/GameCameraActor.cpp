// Fill out your copyright notice in the Description page of Project Settings.

#include "GameCameraActor.h"
#include "Kismet/GameplayStatics.h"
#include "GameCameraComponent.h"
#include "Camera/CameraComponent.h"
#include "../InputManager.h"
#include "../WaterSurface/WaterSurface.h"


// Sets default values
AGameCameraActor::AGameCameraActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// �J�����u�[���𐶐�
	GameCameraBoom = CreateDefaultSubobject<UGameCameraComponent>(TEXT("GameCameraBoom"));
	GameCameraBoom->SetupAttachment(RootComponent);

	// �J�����𐶐�
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	Camera->SetupAttachment(GameCameraBoom);
	Camera->bUsePawnControlRotation = false;

	// �J�����u�[���̃����o�ɃA�^�b�`
	GameCameraBoom->SetCamera(Camera);
}

// Called when the game starts or when spawned
void AGameCameraActor::BeginPlay()
{
	Super::BeginPlay();
	
	AWaterSurface * Water = Cast<AWaterSurface>(UGameplayStatics::GetActorOfClass(GetWorld(), AWaterSurface::StaticClass()));
	FVector Center = Water->GetCenterPos();
	SetActorLocation(Center);
	// ���߂̈ʒu���X�e�[�W�̒��S�Ƃ��ĕێ����Ă���
	FieldCenterPos = Center;
	float Distance = FVector::Dist(Water->GetStartPos(), Center);
	this->FieldDistance = Distance * 1.2f;

	// �J�����̃r���[�|�[�g���Z�b�g
	APlayerController *playerControtller = UGameplayStatics::GetPlayerController(this, 0);
	if (playerControtller)
	{
		playerControtller->SetViewTarget(this);
	}
}

void AGameCameraActor::InputChangeType()
{
	auto IM = AInputManager::GetInstance();
	if (!IM) return;

	auto input = IM->GetState();
	if (input->RightStickButton.IsPress)
	{
		int value = (int)Type + 1;
		if (value >= (int)FollowType::Num) value = 0;
		Type = (FollowType)value;
	}
}

// Called every frame
void AGameCameraActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	InputChangeType();

	float FollowDist = 0.0f;
	FVector FollowPos;

	switch (Type)
	{
	case FollowType::FieldCenter:
		FollowDist = FieldDistance;
		FollowPos = FieldCenterPos;
		break;
	case FollowType::CharacterFollow_Far:
		FollowDist = CharacterDistance_Far;
		if(FollowTarget) FollowPos = FollowTarget->GetActorLocation();
		break;
	case FollowType::CharacterFollow_Near:
		FollowDist = CharacterDistance_Near;
		if (FollowTarget) FollowPos = FollowTarget->GetActorLocation();
		break;
	}

	// �Ȃ߂炩�ɒǏ]
	SetActorLocation(FMath::Lerp<FVector>(GetActorLocation(), FollowPos, 0.1f));
	// �J�����̋������Z�b�g
	GameCameraBoom->SetTargetDistance(FollowDist);
}

