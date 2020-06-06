// Fill out your copyright notice in the Description page of Project Settings.

#include "GoalComponent.h"
#include "../WaterSurface/FloatActor.h"
#include "GameController.h"
#include "Kismet/GameplayStatics.h"

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
		if (isGoal)
			game->MinusGoalCount();

		game->MinusNotExplotionCount();
		isGoal = false;
		isExplotion = true;
		OtherFloat->Destroy();
		//BreakHome();
		return;
	}

	if (isGoal) return;

	if (OtherFloat->ActorHasTag("Nimotu"))
	{
		// �Փ˂����A�N�^�[���ו��Ȃ�S�[���ς݂ɂ���
		isGoal = true;
		game->AddGoalCount();
		// �Փ˂����ו����폜
		OtherFloat->Destroy();
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

