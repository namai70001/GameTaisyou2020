// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatActor.h"
#include "Kismet/GameplayStatics.h"
#include "WaterSurface.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"

// Sets default values
AFloatActor::AFloatActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AFloatActor::BeginPlay()
{
	Super::BeginPlay();
	
	WaterSurface = Cast<AWaterSurface>((UGameplayStatics::GetActorOfClass(GetWorld(), AWaterSurface::StaticClass())));
	StaticMeshComponent = Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass()));
}

// Called every frame
void AFloatActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ���b�V�����Z�b�g����Ă��Ȃ������牽�����Ȃ�
	if (!StaticMeshComponent) return;

	// �t�B�[���h�̊O�ɏo����
	if ((!WaterSurface->IsInField(GetActorLocation())))
	{
		// �������Z���g�p����
		if (!StaticMeshComponent->IsSimulatingPhysics())
			StaticMeshComponent->SetSimulatePhysics(true);

		// ���b��ɃI�u�W�F�N�g���폜
		FTimerManager& timerManager = GetWorld()->GetTimerManager();
		FTimerHandle handle;
		timerManager.SetTimer(handle, this, &AFloatActor::MyDestroy, 3.0f);
		return;
	}

	// �g�̌X���ɉ����Ĉړ�����
	FVector WavePower = WaterSurface->GetWavePower(GetActorLocation());
	FVector MoveVec = WavePower * FloatSpeed * 0.01f;
	Velocity += MoveVec;
	if (Velocity.Size() > MinFloatWavePower)
		SetActorLocation(WaterSurface->AdjustMoveInWater(this, Velocity, FloatScale));
	Velocity *= 0.98f;

	// �g�̍����ɍ��킹��
	FVector CurPos = GetActorLocation();
	float Height = WaterSurface->GetWaveHeight(CurPos);
	Height = FMath::Lerp(CurPos.Z, Height, 0.1f);

	SetActorLocation(FVector(CurPos.X, CurPos.Y, Height));
}

void AFloatActor::MyDestroy()
{
	this->Destroy();
}
