// Fill out your copyright notice in the Description page of Project Settings.


#include "BreakFloatActor.h"

ABreakFloatActor::ABreakFloatActor()
{
	PrimaryActorTick.bCanEverTick = true;
	Scene = CreateDefaultSubobject <USceneComponent>(TEXT("Scene"));
	RootComponent = Scene;
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("BreakFloatActorCollsion"));
	SphereComp->SetupAttachment(RootComponent);
}

void ABreakFloatActor::BeginPlay()
{
	SphereComp->OnComponentBeginOverlap.AddUniqueDynamic(this, &ABreakFloatActor::OnOverlapBegin);
	IsExplotion = false;

	Super::BeginPlay();
}

// Called every frame
void ABreakFloatActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABreakFloatActor::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AFloatActor* OtherFloat = Cast<AFloatActor>(OtherActor);
	// �ו��ȊO�͔��肵�Ȃ�
	if (IsExplotion) return; //���łɉ��Ă��邩��
	if (!OtherFloat) return;

	if (OtherFloat->ActorHasTag("Bom"))
	{
		IsExplotion = true;
		BreakAnime();
		OtherFloat->Destroy();
	}
}