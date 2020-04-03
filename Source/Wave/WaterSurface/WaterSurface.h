// Fill out your copright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../ProceduralMesh/ProceduralMeshActor.h"
#include "WaterSurface.generated.h"

UCLASS()
class WAVE_API AWaterSurface : public AProceduralMeshActor
{
	GENERATED_BODY()

public:
	AWaterSurface();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void CreateWave(int32 x, int32 y);
	void SetLand(int32 sx, int32 sy, int32 ex, int32 ey);
	void AddPawer(FVector worldPos);

private:
	int32 CalcIndex(int32 x, int32 y);

private:
	float X_Size;
	float Y_Size;

	UPROPERTY(EditAnywhere)
		FVector2D SplitVector = FVector2D(100,100);
	UPROPERTY(EditAnywhere)
		AActor* StartPoint;
	UPROPERTY(EditAnywhere)
		AActor* EndPoint;

	TArray<bool> IsLands;

	TArray<float> CurrentHeights;
	TArray<float> PrevHeights;
	TArray<float> NewHeights;
};