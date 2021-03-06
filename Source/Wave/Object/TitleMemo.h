// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../SaveData.h"
#include "TitleMemo.generated.h"
//タイトル用ステージ選択の伝票クラス
class UTitleMemoUI;
class UUserWidget;
class UWidgetComponent;
class UFadeUI;
UCLASS()
class WAVE_API ATitleMemo : public AActor
{
	GENERATED_BODY()
public:	
	// Sets default values for this actor's properties
	ATitleMemo();
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		FSaveDataStruct Save;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Instanced")
		int MyStageNumber;
	UPROPERTY(EditAnywhere)
		TSubclassOf<UTitleMemoUI> UIClass;
	UPROPERTY(EditAnywhere)
		TSubclassOf<UFadeUI> FadeUIClass;
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		UTitleMemoUI* TitleMemoUI;
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		FString StageLevelPath;
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		void CreateUI();
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		UTexture2D* GetTexture2D(const FName AssetPath);
public:	
	void FadeStart();
	int GetStageNumber() { return MyStageNumber; }
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		UUserWidget* GetUserWidgetObject2(UWidgetComponent* widget)const;
};
