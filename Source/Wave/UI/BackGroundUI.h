// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../SaveData.h"
#include "../SoundManager.h"
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
		UAudioComponent* AudioComponent;
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		FText MaxNimotu;
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		FText InNimotu;
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		FText LimitTime;
	//�X�^���v�p�e�L�X�g
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		FText StampNimotu;
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		FText StampPercent;
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		FSaveDataStruct ResultSaveData;
	//���U���g���������ו����擾
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		int CountMaxInNimotu = 0;
	//���U���g���������ו���0����J�E���g
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		int CountInNimotu = 0;
	//���U���g���̃m���}�̉ו�
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		int CountNormaNimotu = 0;
	//���U���g���̉ו��̍ő吔
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		int CountMaxNumNimotu = 0;
	//�J�E���g�A�j�����I�������true
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		bool IsCountAnimeEnd = false;
	//�X�^���v�A�j�����I�������true
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		bool IsStampAnimeEnd = false;
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		float ResultClearAngle;
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		float ResultNormaAngle;
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		int ResultNormaTime;
	//�S�ẴX�^���v��������Ă�����true��Ԃ�
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		bool GetIsAllStamp() const;
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE	bool GetIsCountAnimeEnd() const { return IsCountAnimeEnd; }
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE	bool GetIsStampAnimeEnd() const { return IsStampAnimeEnd; }
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		bool GetIsStamp(const int index) const;
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void SetTimeLimit(const int time);
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void SetNormaAngle(const float time);
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void SetNeedleAndBG_Material(const float angle);
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void NimotuCountPlayAnimation();
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void SetNormaTime(const int time);
	//�Z�[�u�f�[�^�̎w�肵���X�^���v�t���O��true�ɂ���
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		void SetSaveDataIsStamp(const int index) { ResultSaveData.IsStamp[index] = true;}
};
