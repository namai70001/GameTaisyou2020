// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VersusController.generated.h"
class UBattleResultUI;
UCLASS()
class WAVE_API AVersusController : public AActor
{
	GENERATED_BODY()
public:
	// セクションの登録
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Versus")
		TArray<TSubclassOf<class ASectionController>> Sections;
	UPROPERTY(EditAnywhere)
		TSubclassOf<UBattleResultUI> BattleResultUIClass = nullptr;
	UPROPERTY()
		UBattleResultUI* BattleResultUI = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Versus")
		float SectionCount = 5;

private:
	TArray<ASectionController*> Player1Sections;
	TArray<ASectionController*> Player2Sections;

	float SectionSize = 2000;
	//リザルトUIを作成
	//勝敗が決定したら呼ぶ
	void CreateBattleResultUI();
	void InputBattleResultUI();
public:	
	// Sets default values for this actor's properties
	AVersusController();
	void CreateSections();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void AddSection(int index, int PlayerNumber, int Cnt);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
