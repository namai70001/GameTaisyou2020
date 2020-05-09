// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseUI.generated.h"

/**
 * 
 */
UCLASS()
class WAVE_API UPauseUI : public UUserWidget
{
	GENERATED_BODY()
private:
	int SelectNumber;
protected:
	//�����ň�v����ԍ��Ɠ����Z���N�g�ԍ��ɂȂ�����F��ς���
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FSlateColor SelectTextColor(const int number);
	//�A�j���[�V�����Đ����̓|�[�Y�e�L�X�g��\�����Ȃ�
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		ESlateVisibility GetPauseTextVisibility();
public:
	virtual void NativeConstruct() override;
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		void AddSelectNumber();
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		void MinusSelectNumber();
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE	int GetSelectNumber() { return SelectNumber; };
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		bool IsPlayAnimation;//�A�j���[�V�����Đ����Ȃ�true
	//�Ăяo������BP�ŃA�j���[�V�����Đ�
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void InitPlayAnimation();
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void EndPlayAnimation();
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE	bool GetIsPlayAnimation() const{ return IsPlayAnimation; };
};
