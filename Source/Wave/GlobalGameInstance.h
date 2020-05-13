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
public:
	static UGlobalGameInstance* GetInstance();
	UPROPERTY(BlueprintReadWrite, Category = "GlobalC++Class")
		int StageNumber;
};
