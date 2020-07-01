// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BattleController.generated.h"

class UHammerCountUI;
class UGameOverUI;
class UResultUI;
class UGameTimeUI;
class APlayerCharacter;
class UNimotuCountUI;
class UPauseUI;
class UFadeUI;
class UControlTipsUI;

UCLASS()
class WAVE_API ABattleController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABattleController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//��������
	UPROPERTY(EditAnywhere, Category = "Game")
		int TimeLimit = 60;
	//�J�E���g�_�E���\���J�n����
	UPROPERTY(EditAnywhere, Category = "Game")
		int CountDownTime = 10;
	//�X�N���[���V���b�g�p�f�o�b�O�t���O
	UPROPERTY(EditAnywhere, Category = "Game")
		bool DebugScreenMode = false;
	//��������\���t���O
	UPROPERTY(EditAnywhere, Category = "Game")
		bool IsTips = false;
	// BGM�p
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
		class UAudioComponent* AudioComponent;
private:
	APlayerCharacter* GetPlayer1; // �v���C���[1���
	APlayerCharacter* GetPlayer2; // �v���C���[2���
	FVector Player1RespawnLocation;
	FVector Player2RespawnLocation;
	int Player1Score = 0;
	int Player2Score = 0;

	bool IsBatlleFinish = false;
	bool IsPause = false;
	bool IsResult = false;
	bool IsStartResultEvent = false;

	// �\������UI�@�G�f�B�^�Ŏw�肷��
	// TODO::�o�g���p���U���gUI,�o�g���p����UI,�o�g���p�|�[�YUI���K�v
	UPROPERTY(EditAnywhere)
		TSubclassOf<UFadeUI> FadeUIClass;
	UPROPERTY(EditAnywhere)
		TSubclassOf<UControlTipsUI> ControlTipsUIClass;

	// ���X�|�[���p�ɂ�������ێ�
	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> RaftActor;

	//����ɊJ������邱�Ƃ�����̂ŃK�x�[�W�R���N�V�����̑ΏۊO�ɂ���
	UPROPERTY()
		UPauseUI* PauseUI = nullptr;
	UPROPERTY()
		UGameTimeUI* GameTimeUI = nullptr;
	UPROPERTY()
		UGameOverUI* GameOverUI = nullptr;
	UPROPERTY()
		UResultUI* ResultUI = nullptr;
	UPROPERTY()
		UControlTipsUI* ControlTipsUI = nullptr;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "C++Library")
		float GetNowTimeAngle();

	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE int GetTimeLimit() const { return TimeLimit; }

	//�������Ԃ��Z�b�g
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		void SetTime(const int time) { TimeLimit = time;}

	//�^�C���J�E���g�A�j�����Đ�����Ă�����ꎞ��~����
	void SetTimeCountPause();
	//�^�C���J�E���g�A�j�����~�܂��Ă����瑱������Đ�����
	void SetTimeCountRePlay();

	// �|�[�Y�Ăяo��
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		void PauseCall();

	//�@�|�[�Y�⃊�U���g���ɕK�v�ȓ��͊֐�
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		void InputRightCall();
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		void InputLeftCall();
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		void InputSelectCall();

	UFUNCTION(BlueprintCallable, Category = "C++Library")
		void StopBGM();
private:

	// �������UI���쐬
	void CreateControlTipsUI();
	//�Q�[���J�n���̃t�F�[�h�A�E�g
	void InitFadeOut();
	//���v�̐j��i�߂�
	void UpdateTime();
	//�J�E���g�_�E���̎c�莞��0�̎�true��Ԃ�
	bool GetLimitTimeZero();
	//�\�����Ă���UI���\���ɂ���
	void RemoveUI();
	// �X�e�[�W��̃A�C�R�����\��
	void SetAllInvisibleStageIcon();

	// �v���C���[�����������𔻒肵�ăX�R�A�⃊�X�|�[���̏������s��
	void CheckPlayerFall();
	// �o�g�����I������
	void BattleFinish();
};
