// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VersusController.generated.h"
class UHammerCountUI;
class UGameOverUI;
class UResultUI;
class UGameTimeUI;
class APlayerCharacter;
class UNimotuCountUI;
class UPauseUI;
class UFadeUI;
class UControlTipsUI;
class UBattleTimeUI;
class UBattleResultUI;
class UBattleGameStartUI;
UCLASS()
class WAVE_API AVersusController : public AActor
{
	GENERATED_BODY()
public:
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

	bool IsBattleStart = false;
	bool IsBatlleFinish = false;
	bool IsResult = false;
	bool IsStartResultEvent = false;

	float StartIdleTimer = 0.0f;

	// �\������UI�@�G�f�B�^�Ŏw�肷��
	// TODO::�o�g���p���U���gUI,�o�g���p����UI,�o�g���p�|�[�YUI���K�v
	UPROPERTY(EditAnywhere)
		TSubclassOf<UFadeUI> FadeUIClass;
	UPROPERTY(EditAnywhere)
		TSubclassOf<UControlTipsUI> ControlTipsUIClass;
	UPROPERTY(EditAnywhere)
		TSubclassOf<UBattleResultUI> BattleResultUIClass = nullptr;
	UPROPERTY(EditAnywhere)
		TSubclassOf<UBattleGameStartUI> BattleGameStartUIClass = nullptr;

	//����ɊJ������邱�Ƃ�����̂ŃK�x�[�W�R���N�V�����̑ΏۊO�ɂ���
	UPROPERTY()
		UPauseUI* PauseUI = nullptr;
	UPROPERTY()
		UBattleResultUI* BattleResultUI = nullptr;
	UPROPERTY()
		UGameTimeUI* GameTimeUI = nullptr;
	UPROPERTY()
		UGameOverUI* GameOverUI = nullptr;
	UPROPERTY()
		UResultUI* ResultUI = nullptr;
	UPROPERTY()
		UControlTipsUI* ControlTipsUI = nullptr;
	UPROPERTY()
		UBattleGameStartUI* BattleGameStartUI = nullptr;
	UPROPERTY()
		UFadeUI* FadeUI = nullptr;
public:

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
	//���U���gUI���쐬
	void CreateBattleResultUI();
	//�J�n���̃J�E���g�_�E���쐬
	void CreateBattleGameStartUI();
	//�Q�[���J�n���̃t�F�[�h�A�E�g
	void InitFadeOut();
	//�\�����Ă���UI���\���ɂ���
	void RemoveUI();
	// �X�e�[�W��̃A�C�R�����\��
	void SetAllInvisibleStageIcon();

	// �Q�[���J�n�֐�
	void GameStart();

	// �v���C���[�����������𔻒肵�ăX�R�A�⃊�X�|�[���̏������s��
	void CheckPlayerFall();
	// �o�g�����I������
	void BattleFinish();
public:
	// �Z�N�V�����̓o�^
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Versus")
		TArray<TSubclassOf<class ASectionController>> Sections;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Versus")
		float SectionCount = 5;

	void Goal(int BattleNumber);

private:
	TArray<ASectionController*> Player1Sections;
	TArray<ASectionController*> Player2Sections;

	float SectionSize = 2000;
	//���U���gUI���쐬
	//���s�����肵����Ă�
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
