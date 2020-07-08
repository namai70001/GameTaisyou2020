// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BattleGameStartUI.generated.h"

/**
 * 
 */
UCLASS()
class WAVE_API UBattleGameStartUI : public UUserWidget
{
	GENERATED_BODY()
public:
	//�J�E���g��0�ɂȂ�����true
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		bool IsCountDownZero = false;
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE	bool GetIsCountDownZero() const { return IsCountDownZero; }
};
