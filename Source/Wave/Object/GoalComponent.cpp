// Fill out your copyright notice in the Description page of Project Settings.

#include "GoalComponent.h"
#include "../WaterSurface/FloatActor.h"
#include "GameController.h"
#include "Kismet/GameplayStatics.h"
#include "../Camera/GameCameraFocusPoint.h"
#include "../SoundManager.h"
#include "Components/MaterialBillboardComponent.h"

// Sets default values for this component's properties
UGoalComponent::UGoalComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

//	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("GoalCollision"));
}


// Called when the game starts
void UGoalComponent::BeginPlay()
{
	Super::BeginPlay();

	isGoal = false;
	
//	SphereComp->OnComponentBeginOverlap.AddUniqueDynamic(this, &UGoalComponent::OnOverlapBegin);
//	SphereComp->SetupAttachment(this);
}

void UGoalComponent::OnFloatActorCheck(AActor * OtherActor)
{
	AFloatActor* OtherFloat = Cast<AFloatActor>(OtherActor);
	// �ו��ȊO�͔��肵�Ȃ�
	if (isExplotion) return; //���łɉ��Ă��邩��
	if (!OtherFloat) return;
	AGameController* game = Cast<AGameController>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameController::StaticClass()));
	if (!game) return;

	//�Q�[���I�������𖞂����Ă����瓖���蔻����s��Ȃ�
	if (game->GetIsClear() || game->GetIsGameOver())return;

	// �Փ˂����A�N�^�[�����e�̎��������ăS�[���ς݂ł��S�[�����Ă��Ȃ����Ƃɂ���
	if (OtherFloat->ActorHasTag("Bom"))
	{
		ASoundManager::SafePlaySound(SOUND_TYPE::EXPLOSION);
		SetGoalMinus();
		OtherFloat->Destroy();
		AGameCameraFocusPoint::SpawnFocusPoint(GetOwner(), GetOwner()->GetActorLocation());
		SetInvisibleBillbord();
		//BreakHome();
		return;
	}

	if (isGoal) return;

	if (OtherFloat->ActorHasTag("Nimotu"))
	{
		// �Փ˂����A�N�^�[���ו��Ȃ�S�[���ς݂ɂ���
		ASoundManager::SafePlaySound(SOUND_TYPE::GOAL);
		isGoal = true;
		game->AddGoalCount();
		game->MinusGameMaxNimotu();
		// �Փ˂����ו����폜
		OtherFloat->Destroy();
		SetInvisibleBillbord();

		AGameCameraFocusPoint::SpawnFocusPoint(GetOwner(), GetOwner()->GetActorLocation());
		// �����Ńh�A���܂�A�j���[�V�����J�n
		//PlayAnimationDoorClose();
	}
}

// Called every frame
void UGoalComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UGoalComponent::SetInvisibleBillbord()
{
	TArray<UActorComponent*> Billboards;
	GetOwner()->GetComponents(UMaterialBillboardComponent::StaticClass(), Billboards);

	for (auto Billboard : Billboards)
	{
		Cast<UMaterialBillboardComponent>(Billboard)->SetRenderInMainPass(false);
	}
}

void UGoalComponent::SetGoalMinus()
{
	AGameController* game = Cast<AGameController>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameController::StaticClass()));
	if (!game) return;
	if (isGoal)
		game->MinusGoalCount();

	isGoal = false;
	isExplotion = true;
	game->MinusNotExplotionCount();
}

