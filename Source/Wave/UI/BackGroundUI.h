// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../SaveData.h"
#include "BackGroundUI.generated.h"

/**
 * 
 */
UCLASS()
class WAVE_API UBackGroundUI : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		FText MaxNimotu;
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		FText InNimotu;
	//�X�^���v�p�e�L�X�g
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		FText StampNimotu;
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		FText StampPercent;
};
