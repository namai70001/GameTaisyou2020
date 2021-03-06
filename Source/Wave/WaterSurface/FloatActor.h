// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatActor.generated.h"


UENUM()
enum class FloatType
{
	Circle,
	Square
};

class AWaterSurface;
class ACircleLand;
class ASquareLand;

UCLASS()
class WAVE_API AFloatActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloatActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual bool ShouldTickIfViewportsOnly() const override { return true; };
	virtual void DebugDraw() {};

	void MyDestroy();

	UPROPERTY(EditAnywhere)
		float MinFloatWavePower = 0.1f;
	UPROPERTY(EditAnywhere)
		float FloatSpeed = 0.7f;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "1.0"))
		float Friction = 0.02f;	// 摩擦
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "1.0"))
		float Repulsion = 1.0f;

	UPROPERTY(EditAnywhere)
		bool Respawn = false;

	FVector ResetLocation;
	void Reset();
	
	UPROPERTY(EditAnywhere)
		TEnumAsByte<FloatType> Type = FloatType::Circle;

	// FloatType::Circleの時に使う
	UPROPERTY(EditAnywhere)
		float Radius = 40.0f;

	// FloatType::Squareの時に使う
	UPROPERTY(EditAnywhere)
		float XLength = 200.0f;
	UPROPERTY(EditAnywhere)
		float YLength = 200.0f;

	UFUNCTION(BlueprintCallable, Category = "C++Library")
		void SetDeth() { IsDeth; };

	bool IsFall = false;
	bool IsDeth = false;
public:	
	FVector Velocity;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FVector AdjustMove_VS_Circle(const FVector & OldPos, FVector MovedPos, FVector& MoveVec, float CircleRadius);
	FVector AdjustMove_VS_Square(const FVector & OldPos, FVector MovedPos, FVector& MoveVec, float XLen, float YLen);

	float GetRadius() { return Radius; }
	float GetXLength() { return XLength; }
	float GetYLength() { return YLength; }
	//FloatType GetType() { return Type; }

	bool GetIsDeth() { return IsDeth; };
	bool GetIsFall() { return IsFall; };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
	UAudioComponent* FloatAudio;
	   
	UFUNCTION(BlueprintCallable, Category = "Game")
		void StopFallSound();

protected:
	AWaterSurface* WaterSurface;
	bool IsFloating = false;
private:
	bool IsTick = true;
};
