// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SaveData.h"
#include "GlobalGameInstance.generated.h"

// �O���[�o���ϐ��N���X
//----------------------------------------------------
// �V�[�����ׂ��ŕێ�����ϐ���֐����������炲���R�ɒǉ��ǂ���
//----------------------------------------------------
//�Ăяo����
// UGlobalGameInstance* instance = UGlobalGameInstance::GetInstance();
UCLASS()
class WAVE_API UGlobalGameInstance : public UGameInstance
{
	GENERATED_BODY()
private:
		int StageNumber;//�I���X�e�[�W�ԍ�
		bool IsStageSelectMode = false;//true�Ń^�C�g����ʂ��΂��X�e�[�W�I���ɂ���
public:
	static UGlobalGameInstance* GetInstance();
	UFUNCTION(BlueprintCallable, Category = "GlobalC++Class")
		void SetStageNumber(const int number) { StageNumber = number; };
	UFUNCTION(BlueprintCallable, Category = "GlobalC++Class")
		FORCEINLINE int GetStageNumber() const { return StageNumber; }
	UFUNCTION(BlueprintCallable, Category = "GlobalC++Class")
		FORCEINLINE bool GetIsStageSelectMode() const { return IsStageSelectMode; }
	void SetIsStageSelectMode(const bool set) { IsStageSelectMode = set; }
	void StageSelectChenge();//�X�e�[�W�Z���N�g��ʂɈڍs
};
