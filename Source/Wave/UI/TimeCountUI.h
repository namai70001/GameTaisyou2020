// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TimeCountUI.generated.h"

/**
 * 
 */
UCLASS()
class WAVE_API UTimeCountUI : public UUserWidget
{
	GENERATED_BODY()
private:
	// 終了時に呼ばれる（RemoveFromParentでも呼ばれる）
	virtual void NativeDestruct() override;
protected:
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		int TimeCount = 0;
	//カウントが0になったらtrue
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		bool IsCountZero = false;
	//InitAnimation中true
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		bool IsInitAnimation = false;
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		FText CountText;
public:
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		void SetTimeCount(const int time) { TimeCount = time; }
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE	bool GetIsCountZero() const { return IsCountZero; }
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE	bool GetIsInitAnimation() const { return IsInitAnimation; }
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void AnimationPause();
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void AnimationRePlay();
};
