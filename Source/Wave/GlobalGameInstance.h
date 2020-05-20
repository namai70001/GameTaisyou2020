// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
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
		int StageNumber;
public:
	static UGlobalGameInstance* GetInstance();
	//�A�j���[�V�����Đ����̓|�[�Y�e�L�X�g��\�����Ȃ�
	UFUNCTION(BlueprintCallable, Category = "GlobalC++Class")
		void SetStageNumber(const int number) { StageNumber = number; };
	UFUNCTION(BlueprintCallable, Category = "GlobalC++Class")
		FORCEINLINE int GetStageNumber() const { return StageNumber; }
};
