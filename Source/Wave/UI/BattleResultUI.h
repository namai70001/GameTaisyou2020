// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BattleResultUI.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class BattleResultState : uint8
{
	RESTART,
	STAGESELECT,
};
UCLASS()
class WAVE_API UBattleResultUI : public UUserWidget
{
	GENERATED_BODY()
private:
	int SelectNumber = 0;
	bool IsNoInput = false;//true���C���v�b�g���󂯕t���Ȃ�
public:
	//1P����������true
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void SetWinPlayer(const bool player_one_win);
	void NextSelectState();
	void BackSelectState();
	//�Ή�����I���X�e�[�g�̃A�N�V���������s����
	void SelectStateAction();
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE	int GetSelectNumber() const { return SelectNumber; };
	//�L�[���������Ƃ����ډ摜�T�C�Y��ύX
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void ImageSizeChenge();
	//�t�F�[�h�C�����s�����g���C����
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void RetryScene();
	//�t�F�[�h�C�����s���X�e�[�W�I����ʂɖ߂�
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void StageSelectChenge();
};
