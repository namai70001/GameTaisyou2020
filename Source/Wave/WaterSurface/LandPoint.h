// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LandPoint.generated.h"

UENUM()
enum class ELandType
{
	Grass,
	Num	UMETA(Hidden)
};

static const FColor LAND_TYPE_COLORS[] =
{
	FColor::Green,
	FColor::Transparent
};

UCLASS()
class WAVE_API ALandPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALandPoint();

	// 崖にしたければfalse
	UPROPERTY(EditAnywhere)
	bool IsLand = true;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual bool ShouldTickIfViewportsOnly() const override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void DebugDraw() {};

	int GetLandTypeNumber() { return static_cast<int>(LandType.GetValue()); }
	
	// 渡された引数の座標と接しているかどうか
	virtual bool OnGround(const FVector & Pos) { return false; };
	virtual bool OnGround(const FVector & Pos, float CircleRadius) { return false; };
	// 地面の中に完全に入っているか
	virtual bool InGround(const FVector & Pos, float CircleRadius) { return false; };
	// この地面の外に出ないように移動する際のチェック
	virtual FVector AdjustMoveInLand(const FVector & Pos, float CircleRadius) { return FVector::ZeroVector; };
	virtual FVector AdjustMoveOutWater(const FVector & OldPos, FVector MovedPos, FVector & MoveVec, float CircleRadius, float Repulsion) { return FVector::ZeroVector; };
	virtual FVector AdjustMoveOutWater(const FVector & OldPos, FVector MovedPos, FVector & MoveVec, float XLen, float YLen, float Repulsion) { return FVector::ZeroVector; };

	void SetIsUse(bool isUse) { IsUse = isUse; }

	UFUNCTION(BlueprintCallable, Category = "C++Library")
	bool GetIsUse() { return IsUse; }


	UFUNCTION(BlueprintCallable, Category = "C++Library")
		bool GetIsLand() const { return IsLand; }

private:
	UPROPERTY(EditAnywhere)
		TEnumAsByte<ELandType> LandType = ELandType::Grass;

	bool IsUse = true; // falseにすることで陸でなくすることができる（橋の処理に使う）
};
