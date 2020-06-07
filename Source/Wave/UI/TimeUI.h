// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TimeUI.generated.h"

/**
 * 
 */
UCLASS()
class WAVE_API UTimeUI : public UUserWidget
{
	GENERATED_BODY()
private:

public:
	//��������
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		int LimitTime;
	//1�t���[���ŉ�]�����
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		float OneFrameAngle;
	//�J�E���g�_�E��UI��\�������]�l
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		float CountDownAngle;
	//�m���}��\�������]�l
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		float NormaAngle;
	//1�t���[�����ɉ�]����l�����߂�
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		void SetOneFrameAngle(const int time_limit);
	//�J�E���g�_�E��UI��\�����鎞�Ԃ��Z�b�g
	//�߂�l�͈��������̂܂ܕԂ�
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		int SetCountDownAngle(const int countdown);
	//�m���}�j�̉�]�l���Z�b�g
	//�߂�l�͉�]�l
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		float SetNormaAngle(const int norma_time);
};
