// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseUI.generated.h"
UENUM(BlueprintType)
enum class PauseSelectState : uint8
{
	GAMEBACK,
	RESTART,
	STAGESELECT
};

UCLASS()
class WAVE_API UPauseUI : public UUserWidget
{
	GENERATED_BODY()
private:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	int SelectNumber = 0;
	bool IsNoInput;//true���C���v�b�g���󂯕t���Ȃ�
protected:
	//�A�j���[�V�����Đ����̓|�[�Y�e�L�X�g��\�����Ȃ�
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		ESlateVisibility GetPauseTextVisibility();
public:
	void NextSelectState();
	void BackSelectState();
	//�Ή�����I���X�e�[�g�̃A�N�V���������s����
	void SelectStateAction();
	void EndAnimation();
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE	int GetSelectNumber() const{ return SelectNumber; };
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		bool IsPlayAnimation;//�A�j���[�V�����Đ����Ȃ�true
	//�Ăяo������BP�ŃA�j���[�V�����Đ�
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void InitPlayAnimation();
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void EndPlayAnimation();
	//�|�[�Y��ʂ̎��v�̂��ߐ������Ԃ��󂯎��
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void SetTimeLimit(const int time);
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void SetNormaTime(const int time);
	//�|�[�Y��ʂ̎��v�̂��߃m���}�̐j�̈ʒu���󂯎��
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void SetNormaAngle(const float time);
	//�|�[�Y��ʂ̎��v�̂��ߌ��݂̐j�̈ʒu���󂯎��
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void SetNeedleAndBG_Material(const float angle);
	//�t�F�[�h�C�����s�����g���C����
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void Retry();
	//�t�F�[�h�C�����s���X�e�[�W�I����ʂɖ߂�
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void StageSelectChenge();
	//�L�[���������Ƃ����ډ摜�T�C�Y��ύX
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void ImageSizeChenge();
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE	bool GetIsPlayAnimation() const{ return IsPlayAnimation; };
};
