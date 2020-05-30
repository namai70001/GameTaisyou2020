// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseUI.generated.h"
UENUM(BlueprintType)
enum class PauseState : uint8
{
	GAMEBACK,
	RESTART,
	SCORE,
	STAGESELECT,
};

UCLASS()
class WAVE_API UPauseUI : public UUserWidget
{
	GENERATED_BODY()
private:
	virtual void NativeConstruct() override;
	int SelectNumber;
	bool IsNoInput;//true���C���v�b�g���󂯕t���Ȃ�
protected:
	//�����ň�v����ԍ��Ɠ����Z���N�g�ԍ��ɂȂ�����F��ς���
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FSlateColor SelectTextColor(const PauseState state);
	//�A�j���[�V�����Đ����̓|�[�Y�e�L�X�g��\�����Ȃ�
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		ESlateVisibility GetPauseTextVisibility();
public:
	void NextSelectState();
	void BackSelectState();
	//�Ή�����I���X�e�[�g�̃A�N�V���������s����
	void SelectStateAction();
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE	int GetSelectNumber() const{ return SelectNumber; };
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		bool IsPlayAnimation;//�A�j���[�V�����Đ����Ȃ�true
	//�Ăяo������BP�ŃA�j���[�V�����Đ�
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void InitPlayAnimation();
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void EndPlayAnimation();
	//�X�^���v�e�X�g�p�C�x���g
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void TestStampPlayAnimation();
	//�|�[�Y��ʂ̃o�[���ߌ��݂̃v���C���[�̍ő�HP���󂯎��
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void SetMaxHP(const float hp);
	//�|�[�Y��ʂ̃o�[�X�V�̂��ߌ��݂̃v���C���[HP���󂯎��
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void SetHP(const float hp);
	//�t�F�[�h�C�����s�����g���C����
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void Retry();
	//�t�F�[�h�C�����s���X�e�[�W�I����ʂɖ߂�
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void StageSelectChenge();
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE	bool GetIsPlayAnimation() const{ return IsPlayAnimation; };
};
