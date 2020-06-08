// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameController.generated.h"

class UHammerCountUI;
class UGameOverUI;
class UResultUI;
class UGameTimeUI;
class APlayerCharacter;
class UNimotuCountUI;
class UPauseUI;
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
	//��������
	UPROPERTY(EditAnywhere, Category = "Game")
		int TimeLimit = 60;
	//�m���}����
	UPROPERTY(EditAnywhere, Category = "Game")
		int NormaTime = 10;
	//�J�E���g�_�E���\���J�n����
	UPROPERTY(EditAnywhere, Category = "Game")
		int CountDownTime = 10;
	// BGM�p
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
		UAudioComponent* AudioComponent;
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

	// �\������UI�@�G�f�B�^�Ŏw�肷��
	UPROPERTY(EditAnywhere)
		TSubclassOf<UGameTimeUI> GameTimeUIClass;
	UPROPERTY(EditAnywhere)
		TSubclassOf<UGameOverUI> GameOverUIClass;
	UPROPERTY(EditAnywhere)
		TSubclassOf<UResultUI> ResultUIClass;
	UPROPERTY(EditAnywhere)
		TSubclassOf<UNimotuCountUI> NimotuCountUIClass;
	UPROPERTY(EditAnywhere)
		TSubclassOf<UPauseUI> PauseUIClass;
	UPauseUI* PauseUI = nullptr;
	UGameTimeUI* GameTimeUI = nullptr;
	UGameOverUI* GameOverUI = nullptr;
	UResultUI* ResultUI = nullptr;
	UNimotuCountUI* NimotuCountUI = nullptr;
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
		float GetNowTimeAngle();
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		float GetNormaTimeAngle();
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE int GetNormaTime() const { return NormaTime; }
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE int GetTimeLimit() const { return TimeLimit; }
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		void SetNormaTime(const int time) { NormaTime = time; }
	//�������Ԃƃm���}���Ԃ��Z�b�g
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		void SetTime(const int time, const int norma) { TimeLimit = time; NormaTime = norma; }
	//�Q�[���I�[�o�[UI�𐶐����Ă�����true
	bool GetIsGameOverUI();
	void MinusGameMaxNimotu();
	//�f�[�^�e�[�u������Ή�����X�e�[�W�ԍ�����
	//�Q�[�W�̃p�[�Z���g�ƃN���A�ɕK�v�ȉו����擾�i�f�o�b�O�̎��͓ǂݍ��܂Ȃ��j
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void DataTableLoad();
	//�^�C���J�E���g�A�j�����Đ�����Ă�����ꎞ��~����
	void SetTimeCountPause();
	//�^�C���J�E���g�A�j�����~�܂��Ă����瑱������Đ�����
	void SetTimeCountRePlay();
	void AddGoalCount();
	void MinusGoalCount();
	void AddNotExplotionCount() { NotExplotionCount++; }
	void MinusNotExplotionCount() { NotExplotionCount--; }
private:

	void CreateGameTimeUI();
	void CreateGameOverUI();
	void CreateResultUI();
	void CreateNimotuCountUI();
	void InputGameOverUI();
	void InputResultUI();
	void InputPause();
	//���v�̐j��i�߂�
	void UpdateTime();
	//�Q�[���N���A�����m�F
	void GameClearCheck();
	//�Q�[���I�[�o�[�����m�F
	void GameOverCheck();
	void GameClear();
	void GameOver();
	//�Q�[���v���C���̉ו����m�F
	int CountGameNimotu();
	//�J�E���g�_�E���̎c�莞��0�̎�true��Ԃ�
	bool GetLimitTimeZero();
};
