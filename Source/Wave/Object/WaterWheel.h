// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaterWheel.generated.h"
//�g���󂯂����鐅�ԃN���X
class AWaterSurface;
UCLASS()
class WAVE_API AWaterWheel : public AActor
{
	GENERATED_BODY()

private:
	bool IsRotation;//��]��true
	float WheelPower;
	float RotationTime;
	float Rot;
	float firstrot;
	bool IsUpRot;//��]�l�����̒l��������true
	AWaterSurface* WaterSurface;
	float CheckFirstWheelRotation(FVector pos);
	void CheckReverseWheelRotation(FVector pos);//����Ă��鎞���Ε����֋����͂�������ꂽ�甽�΂ɉ�
public:	
	// Sets default values for this actor's properties
	AWaterWheel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		float RotationTimeCheck(FVector pos);
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE bool GetIsRotation() const { return IsRotation; }
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE float GetWheelPower() const { return WheelPower; }
};
