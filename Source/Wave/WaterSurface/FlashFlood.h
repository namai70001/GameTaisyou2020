// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FlashFlood.generated.h"

UCLASS()
class WAVE_API AFlashFlood : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFlashFlood();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual bool ShouldTickIfViewportsOnly() const override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool CheckRange(FVector worldPos);	// ���W���͈͓����ǂ���
	FVector GetFloatVec(FVector worldPos);	// ���W����͈͓��Ȃ�����x�N�g����Ԃ�
	FVector GetFloatVector() { return GetActorForwardVector(); };	// ��������

	float GetCurrentTime() { return CurrentTime; }

	float GetHeight(const FVector & worldPos);

public:
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "10.0"))
		float SwingWeight = 1.0f;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"))
		float MaxHight = 30.0f;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"))
		float Speed = 5.0f;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"))
		float Power = 1.0f;

private:
	float CurrentTime = 0.0f; // ���Ԍv���p
	float ScaleInterpolation = 100.0f;
};
