// Fill out your copyright notice in the Description page of Project Settings.


#include "Goal.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/UObjectGlobals.h"
#include "Blueprint/UserWidget.h"
#include "../UI/HammerCountUI.h"
#include "../WaterSurface/FloatActor.h"

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

	// �Փ˂����A�N�^�[���ו��Ȃ�S�[���ς݂ɂ���
	AFloatActor* OtherFloat = Cast<AFloatActor>(OtherActor);
	if (OtherFloat)
	{
		if (OtherFloat->ActorHasTag("Bom")) {
			OtherFloat->Destroy();
			BreakHome();
			return;
		}
	}

	if (isGoal) return;
	
	if (OtherFloat)
	{
		isGoal = true;
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
