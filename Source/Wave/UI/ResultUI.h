// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ResultUI.generated.h"

UENUM(BlueprintType)
enum class ResultState : uint8
{
	RESULT_ANIME,
	CLEAR_INANIME,
	NIMOTU_ANIME,
	GAUGE_ANIME,
	STAMP_ANIME,
	ENTER_WAIT,
	INPUT,
	END,
};
UENUM(BlueprintType)
enum class ResultSelectState2 : uint8
{
	NEXTSTAGE,
	RESTART,
	STAGESELECT,
};
UCLASS()
class WAVE_API UResultUI : public UUserWidget
{
	GENERATED_BODY()
private:
	virtual void NativeConstruct() override;
	int SelectNumber = 0;
protected:
	// BGM用
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C++Class")
		UAudioComponent* AudioComponent;

public:
	void NextSelectState();
	void BackSelectState();
	//対応する選択ステートのアクションを実行する
	void SelectStateAction();
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		FString NextStagePath;
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		bool IsPlayAnimation = false;//アニメーション再生中ならtrue
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		bool IsDataTable = false;//データテーブルを読み込めたらtrue
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		ResultState State;//リザルトのステート状態保持
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		ResultSelectState2 SelectState;
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void SetTextStageNumber(const int number);
	//フェードインを行いリトライする
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void Retry();
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void ClearPlayAnimation();
	//このステージをクリアした時の制限時間、クリア時の針アングル、ノルマ針アングル、ノルマ時間をセット
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void SetResultTokeiAnimeCheckEvent(const int limit_time,const float clear_angle,const float norma_angle,const int norma_time);
	//このステージで取得した荷物をセット
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void SetResultNowNimotuCheckEvent(const int NowNimotu);
	//このステージのスタンプアニメーションに必要な情報をセット
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void SetStampAnimeCheckEvent(const int NormaNimotu,const int MaxNimotu);
	//フェードインを行い次のステージに行く
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void NextStageChenge();
	//フェードインを行いステージ選択画面に戻る
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void StageSelectChenge();
	//データテーブルが存在するか確認
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void CheckDataTable();
	//キーを押したとき項目画像サイズを変更
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void ImageSizeChenge();
	//少し待ってからINPUTステートに切り替え
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void WaitInputStateChenge();
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		void SetStageSelectState();
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE	ResultState GetResultState() const { return State; };
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE	int GetSelectNumber() const { return SelectNumber; };
};
