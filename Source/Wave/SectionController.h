// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SectionController.generated.h"

class UGoalComponent;
class AScaffold;

enum class ESectionState
{
	Challenge,	// ���풆
	Clear,		// �N���A�ς�
	None		// �������Ȃ�
};

UCLASS()
class WAVE_API ASectionController : public AActor
{
	GENERATED_BODY()

public:
	//�@�S�[��
	UPROPERTY(EditAnywhere, Category = "Section")
		TArray<AActor*> GoalActors;

	//�@����
	UPROPERTY(EditAnywhere, Category = "Section")
		TArray<AScaffold*> Scaffolds;

	ESectionState State;

protected:
	bool IsClear;
	
public:	
	// Sets default values for this actor's properties
	ASectionController();

	TArray<UGoalComponent*> Goals;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool CheckClear();

	void OnClear();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
