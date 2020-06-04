// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameController.generated.h"

class UHammerCountUI;
class UGameOverUI;
class UResultUI;
class UTimeCountUI;
class APlayerCharacter;
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
	//�n���}�[��0�ɂȂ������̎c�莞��
	UPROPERTY(EditAnywhere, Category = "Game")
		int LimitTime = 10;
private:
	APlayerCharacter* GetPlayer;//�v���C���[���
	// �Q�[���N���A���ǂ���
	bool IsGameClear;
	//�Q�[���I�[�o�[���ǂ���
	bool IsGameOver;
	//�S�[���ɓ������ו����J�E���g
	int GoalCount = 0;
	//���Ă��Ȃ��Ƃ̐����J�E���g
	int NotExplotionCount = 0;
	//���̃X�e�[�W�̃S�[�������ɓ���ו��̐�
	int MaxNimotu = 0;
	//�Q�[����ɔz�u����Ă���ו��̐��i���A���^�C���X�V�j
	int GameMaxNimotu = 0;
	// �V�[����̑S�ẴS�[���̃|�C���^����������
	TArray<class AGoal*> GoalArray;

	// �\������UI�@�G�f�B�^�Ŏw�肷��
	UPROPERTY(EditAnywhere)
		TSubclassOf<UTimeCountUI> TimeCountUIClass;
	UPROPERTY(EditAnywhere)
		TSubclassOf<UGameOverUI> GameOverUIClass;
	UPROPERTY(EditAnywhere)
		TSubclassOf<UResultUI> ResultUIClass;
	UTimeCountUI* TimeCountUI = nullptr;
	UGameOverUI* GameOverUI = nullptr;
	UResultUI* ResultUI = nullptr;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// �Q�[���N���A���ǂ����𔻒f����֐�
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE bool GetIsClear() const { return IsGameClear; }
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE bool GetIsGameOver() const { return IsGameOver; }
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE int GetGoalCount() const { return GoalCount; }
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		void SetNormaGoalCount(const int goalcount) { NormaGoalCount = goalcount; }
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE int GetNormaGoalCount() const { return NormaGoalCount; }
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		void SetMaxNimotu(const int num) { MaxNimotu = num; };
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		int GetMaxNimotu();
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE float GetNormaPercent() const { return NormaPercent; }
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		void SetNormaPercent(const float percent) { NormaPercent = percent; }
	void MinusGameMaxNimotu() { GameMaxNimotu--; }
	//�f�[�^�e�[�u������Ή�����X�e�[�W�ԍ�����
	//�Q�[�W�̃p�[�Z���g�ƃN���A�ɕK�v�ȉו����擾�i�f�o�b�O�̎��͓ǂݍ��܂Ȃ��j
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void DataTableLoad();
	//�^�C���J�E���g�A�j�����Đ�����Ă�����ꎞ��~����
	void SetTimeCountPause();
	//�^�C���J�E���g�A�j�����~�܂��Ă����瑱������Đ�����
	void SetTimeCountRePlay();
	void AddGoalCount() { GoalCount++; }
	void MinusGoalCount() { GoalCount--; }
	void AddNotExplotionCount() { NotExplotionCount++; }
	void MinusNotExplotionCount() { NotExplotionCount--; }
private:
	// �S�[���ς݂̌����擾
	int GetGoalCount();
	// �����ς݂̉Ƃ̌����擾
	int GetNotExplotionCount();
	void CreateTimeCountUI();
	void CreateGameOverUI();
	void CreateResultUI();
	void InputGameOverUI();
	void InputResultUI();
	//�Q�[�W�̃m���}�ݒ�
	void SetNorma();
	//�Q�[���N���A�����m�F
	void GameClearCheck();
	//�Q�[���I�[�o�[�����m�F
	void GameOverCheck();
	//�Q�[���v���C���̉ו����m�F
	int CountGameNimotu();
	//�n���}�[��HP��0�Ńn���}�[��@���I�������J�E���g�_�E�����J�n����
	void CheckTimeCount();
	//�J�E���g�_�E���̎c�莞��0�̎�true��Ԃ�
	bool GetLimitTimeZero();
};
