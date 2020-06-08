// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ResultUI.generated.h"

UENUM(BlueprintType)
enum class ResultState : uint8
{
	RESULT_ANIME,
	CLEAR_INANIME,
	NIMOTU_ANIME,
	GAUGE_ANIME,
	STAMP_ANIME,
	ENTER_WAIT,
	INPUT,
	END,
};
UENUM(BlueprintType)
enum class ResultSelectState : uint8
{
	NEXTSTAGE,
	STAGESELECT,
};
UCLASS()
class WAVE_API UResultUI : public UUserWidget
{
	GENERATED_BODY()
private:
	virtual void NativeConstruct() override;
	int SelectNumber = 0;
protected:
	// BGM�p
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C++Class")
		UAudioComponent* AudioComponent;

public:
	void NextSelectState();
	void BackSelectState();
	//�Ή�����I���X�e�[�g�̃A�N�V���������s����
	void SelectStateAction();
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		FString NextStagePath;
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		bool IsPlayAnimation = false;//�A�j���[�V�����Đ����Ȃ�true
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		bool IsDataTable = false;//�f�[�^�e�[�u����ǂݍ��߂���true
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		ResultState State;//���U���g�̃X�e�[�g��ԕێ�
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		ResultSelectState SelectState;
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void ClearPlayAnimation();
	//���̃X�e�[�W���N���A�������̐������ԁA�N���A���̐j�A���O���A�m���}�j�A���O���A�m���}���Ԃ��Z�b�g
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void SetResultTokeiAnimeCheckEvent(const int limit_time,const float clear_angle,const float norma_angle,const int norma_time);
	//���̃X�e�[�W�Ŏ擾�����ו����Z�b�g
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void SetResultNowNimotuCheckEvent(const int NowNimotu);
	//���̃X�e�[�W�̃X�^���v�A�j���[�V�����ɕK�v�ȏ����Z�b�g
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void SetStampAnimeCheckEvent(const int NormaNimotu,const int MaxNimotu);
	//�t�F�[�h�C�����s�����̃X�e�[�W�ɍs��
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void NextStageChenge();
	//�t�F�[�h�C�����s���X�e�[�W�I����ʂɖ߂�
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void StageSelectChenge();
	//�f�[�^�e�[�u�������݂��邩�m�F
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void CheckDataTable();
	//�L�[���������Ƃ����ډ摜�T�C�Y��ύX
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void ImageSizeChenge();
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		void SetStageSelectState();
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE	ResultState GetResultState() const { return State; };
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE	int GetSelectNumber() const { return SelectNumber; };
};
