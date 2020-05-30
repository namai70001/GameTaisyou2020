// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameController.generated.h"

class UHammerCountUI;
class UStageClearUI;
class UGameOverUI;
// �Q�[�����̃N���A�����Ȃǂ��Ǘ�����N���X

UCLASS()
class WAVE_API AGameController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// �N���A���ǂ����̃S�[���̃m���}�J�E���g
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
		int NormaGoalCount = 0;
	//�m���}�̊��� 0-100�œ���
	UPROPERTY(EditAnywhere, Category = "Game")
		float NormaPercent = 0;
private:
	// �Q�[���N���A���ǂ���
	bool IsGameClear;
	//�Q�[���I�[�o�[���ǂ���
	bool IsGameOver;
	//�S�[���ɓ������ו����J�E���g
	int GoalCount;
	// �V�[����̑S�ẴS�[���̃|�C���^����������
	TArray<class AGoal*> GoalArray;

	// �\������UI�@�G�f�B�^�Ŏw�肷��
	UPROPERTY(EditAnywhere)
		TSubclassOf<UStageClearUI> StageClearUIClass;
	UPROPERTY(EditAnywhere)
		TSubclassOf<UGameOverUI> GameOverUIClass;
	UStageClearUI* StageClearUI;
	UGameOverUI* GameOverUI;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// �Q�[���N���A���ǂ����𔻒f����֐�
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE bool GetIsClear() const { return IsGameClear; }
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE int GetGoalCount() const { return GoalCount; }
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		void SetNormaGoalCount(const int goalcount) { NormaGoalCount = goalcount; }
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE int GetNormaGoalCount() const { return NormaGoalCount; }
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE float GetNormaPercent() const { return NormaPercent; }
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		void SetNormaPercent(const float percent) { NormaPercent = percent; }
	//�f�[�^�e�[�u������Ή�����X�e�[�W�ԍ�����
	//�Q�[�W�̃p�[�Z���g�ƃN���A�ɕK�v�ȉו����擾�i�f�o�b�O�̎��͓ǂݍ��܂Ȃ��j
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void DataTableLoad();
private:
	// �S�[���ς݂̌����擾
	int GetGoalCount();
	// �����ς݂̉Ƃ̌����擾
	int GetNotExplotionCount();
	void CreateStageClearUI();
	void CreateGameOverUI();
	void InputGameOverUI();
	//�Q�[�W�̃m���}�ݒ�
	void SetNorma();
};
