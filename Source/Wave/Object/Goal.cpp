// Fill out your copyright notice in the Description page of Project Settings.


#include "Goal.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/UObjectGlobals.h"
#include "Blueprint/UserWidget.h"
#include "../UI/HammerCountUI.h"
#include "../WaterSurface/FloatActor.h"
#include "GameController.h"
#include "Kismet/GameplayStatics.h"
// Sets default values
AGoal::AGoal()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Scene = CreateDefaultSubobject <USceneComponent>(TEXT("Scene"));
	RootComponent = Scene;
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("GoalCollision"));
	SphereComp->SetupAttachment(RootComponent);
	
}

// Called when the game starts or when spawned
void AGoal::BeginPlay()
{
	Super::BeginPlay();
	isGoal = false;
	SphereComp->OnComponentBeginOverlap.AddUniqueDynamic(this, &AGoal::OnOverlapBegin);
}

void AGoal::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AFloatActor* OtherFloat = Cast<AFloatActor>(OtherActor);
	// �ו��ȊO�͔��肵�Ȃ�
	if (isExplotion) return; //���łɉ��Ă��邩��
	if (!OtherFloat) return;
	AGameController* game = Cast<AGameController>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameController::StaticClass()));
	//�Q�[���I�������𖞂����Ă����瓖���蔻����s��Ȃ�
	if (game->GetIsClear() || game->GetIsGameOver())return;
	// �Փ˂����A�N�^�[�����e�̎��������ăS�[���ς݂ł��S�[�����Ă��Ȃ����Ƃɂ���
	if (OtherFloat->ActorHasTag("Bom"))
	{
		if (game)
		{
			if (isGoal)
			{
				game->MinusGoalCount();
			}
			game->MinusNotExplotionCount();
		}
		isGoal = false;
		isExplotion = true;
		OtherFloat->Destroy();
		BreakHome();
		return;
	}

	if (isGoal) return;
	
	if (OtherFloat->ActorHasTag("Nimotu"))
	{
		// �Փ˂����A�N�^�[���ו��Ȃ�S�[���ς݂ɂ���
		isGoal = true;
		if (game)
		{
			game->AddGoalCount();
		}
		// �Փ˂����ו����폜
		OtherFloat->Destroy();
		// �����Ńh�A���܂�A�j���[�V�����J�n
		PlayAnimationDoorClose();
	}
}

// Called every frame
void AGoal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
