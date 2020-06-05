// Fill out your copright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../ProceduralMesh/ProceduralMeshActor.h"
#include "WaterSurface.generated.h"

class ALandPoint;
class AFlashFlood;
class ABreakSquareLand;

enum class VertexType
{
	Water,	// ��
	Land,	// ��
	Cliff,	// �R
	None
};

UCLASS()
class WAVE_API AWaterSurface : public AProceduralMeshActor
{
	GENERATED_BODY()

public:
	AWaterSurface();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void AddPower(FVector worldPos, float pawer);
	FVector GetWavePower(const FVector & worldPos);
	float GetWaveHeight(const FVector & worldPos);	// �g�̍������擾
	float GetMaxWaveHeight() { return MaxWaveHight; };	// �g�̍ő卂�����擾
	float GetWaveSpeed() { return WaveSpeed; }
	FVector AdjustMoveInField(const FVector & worldPos, float circleRadius);
	FVector AdjustMoveInWater(const AActor * Object, FVector& moveVec, float circleRadius);
	FVector AdjustMoveInWater(const AActor * Object, FVector& moveVec, float XLen, float YLen);
	FVector AdjustMoveInLand(const FVector & worldPos, const FVector & moveVec, float circleRadius, const FVector & WaterCheckPos, float WaterCheckRadius);
	bool IsInWater(FVector worldPos);
	bool IsInLand(FVector worldPos);
	VertexType GetVertexType(FVector worldPos);
	bool IsInField(FVector worldPos);	// �X�e�[�W�O���ǂ������ׂ�
	bool IsInField(FVector worldPos, float CircleRadius);	// �X�e�[�W�O���ǂ������ׂ�
	FVector GetGetOffPos(FVector WorldPos, float Radius);	// ������~����ꏊ���擾
	ALandPoint * GetLandPoint(const FVector & WorldPos, bool IsLand = true);	// �w�肵�����W�ɐڂ��Ă���n�ʂ��擾
	ALandPoint * GetLandPoint(const FVector & WorldPos, float Radius, bool IsLand = true);	// �w�肵�����W�ɐڂ��Ă���n�ʂ��擾
	FVector GetCenterPos();
	FVector GetStartPos() { return StartPoint->GetActorLocation(); };
private:
	void CreateWave(int32 x, int32 y, float pawer);
	void SetCircleLand(FVector CirclePostion, float Radius, VertexType Type);
	void SetSquareLand(FVector SquareLocation, float XLength, float YLength, VertexType Type);
	void SetLand(int X, int Y, float Z, VertexType Type);

	void TickFlashFloodWave(AFlashFlood* FlashFlood);

	void HammerBreakLand(const FVector & worldPos);

	int32 CalcIndex(int32 x, int32 y);

private:
	FIntPoint SplitPointNum = FIntPoint(0,0);

	UPROPERTY(EditAnywhere, meta = (ClampMin = "5.0", ClampMax = "100.0"))
		float SplitSpace = 10.0f;	// ���_�̊Ԋu
	UPROPERTY(EditAnywhere)
		AActor* StartPoint;
	UPROPERTY(EditAnywhere)
		AActor* EndPoint;
	UPROPERTY(EditAnywhere)
		UMaterialInterface *Material;
	UPROPERTY(EditAnywhere)
		float Decay = 0.3f;
	UPROPERTY(EditAnywhere)
		float WaveSpeed = 10.0f;
	UPROPERTY(EditAnywhere)
		float WavePower = 1.0f;
	UPROPERTY(EditAnywhere)
		float MaxWaveHight = 100.0f;
	UPROPERTY(EditAnywhere)
		FLinearColor WaterColor = FLinearColor::Blue;
	UPROPERTY(EditAnywhere)
		FLinearColor WaveColor = FLinearColor::White;

	TArray<VertexType> VertexTypes;

	TArray<float> CurrentHeights;
	TArray<float> PrevHeights;
	TArray<float> NewHeights;

	TArray<ALandPoint*> LandPointActors;	// �Փˏ����ȂǂɎg���̂Ń����o�ɂ���
	TArray<AFlashFlood*> FlashFloods;		// �����̌v�Z�Ɏg���̂Ń����o�ɂ���
	TArray<ABreakSquareLand*> BreakLands;	// �n���}�[�ŏ��󂷂Ƃ��Ɏg��
};