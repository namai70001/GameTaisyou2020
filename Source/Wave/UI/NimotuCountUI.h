// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NimotuCountUI.generated.h"

/**
 * 
 */
UCLASS()
class WAVE_API UNimotuCountUI : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		FText StageNimotu;
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		FText InNimotu;
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		FText NormaNimotu;
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		FText MaxNimotu;
	void SetStageNimotu(const int count);
	void SetStageInNimotu(const int count);
	void SetNormaNimotu(const int count);
	void SetMaxNimotu(const int count);
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void GameNimotuAnimation();
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void NimotuInAnimation();
	//�m���}��B��������A�j���[�V�������ɕ����F��ԐF�ɂ���
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void NormaInAnimation();
	//�m���}��B�����Ă��ăm���}�ȉ��ɂȂ�����A�j���[�V�������ɕ����F�����ɖ߂�
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void NormaNoAnimation();
};
