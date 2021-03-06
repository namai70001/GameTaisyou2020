// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SaveData.h"
#include "GlobalGameInstance.generated.h"

// グローバル変数クラス
//----------------------------------------------------
// シーンを跨いで保持する変数や関数があったらご自由に追加どうぞ
//----------------------------------------------------
//呼び出し方
// UGlobalGameInstance* instance = UGlobalGameInstance::GetInstance();
UCLASS()
class WAVE_API UGlobalGameInstance : public UGameInstance
{
	GENERATED_BODY()
private:
		int StageNumber = -1;//選択ステージ番号 デバッグ時にデバッグテキストを出すように―1を初期値にする
		bool IsStageSelectMode = false;//trueでタイトル画面を飛ばしステージ選択にする
public:
	static UGlobalGameInstance* GetInstance();
	UFUNCTION(BlueprintCallable, Category = "GlobalC++Class")
		void SetStageNumber(const int number) { StageNumber = number; };
	UFUNCTION(BlueprintCallable, Category = "GlobalC++Class")
		FORCEINLINE int GetStageNumber() const { return StageNumber; }

	UFUNCTION(BlueprintCallable, Category = "GlobalC++Class")
		FORCEINLINE bool GetIsStageSelectMode() const { return IsStageSelectMode; }
	//ゲームパッドが接続されていたらtrueを返す
	UFUNCTION(BlueprintCallable, Category = "GlobalC++Class")
		bool GetIsGamePad() const;

	void SetIsStageSelectMode(const bool set) { IsStageSelectMode = set; }
	UFUNCTION(BlueprintCallable, Category = "GlobalC++Class")
	void StageSelectChenge();//ステージセレクト画面に移行
};
