// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveData.generated.h"
#define STAMP_NUM (3)//�z�B���̃X�^���v�̐�
USTRUCT(BlueprintType)
struct FSaveDataStruct
{
	GENERATED_USTRUCT_BODY();
	FSaveDataStruct(){
		IsStamp.Init(false, STAMP_NUM);
	}
	//�X�e�[�W�ɔz�B������
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Weapon)
		int InNimotu = 0;

	//�X�e�[�W���N���A�������̃Q�[�W�ʒu
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Weapon)
		float HpBarPercent = 0.0f;

	//�X�^���v�̏�ԁA������Ă�����true
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Weapon)
		TArray<bool> IsStamp;

};
UCLASS()
class WAVE_API USaveData : public USaveGame
{
	GENERATED_BODY()
private:
	
public:
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		TArray<FSaveDataStruct> Save;
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		void InSaveData(const int number, const FSaveDataStruct savedata);
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE FSaveDataStruct GetSaveData(const int number) const;
};