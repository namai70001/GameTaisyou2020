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
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		float NowTimeAngle = 0.0f;
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		float NormaTimeAngle = 0.0f;
	//���U���g�Ŏg�p����ϐ�/////
	//���v�̃A�j�����I�������True
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		bool IsTokeiAnimeEnd = false;
	//�N���A���̐j�̈ʒu
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		float ClearAngle;
	//�N���A���̃m���}�j�̈ʒu
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		float ClearNormaAngle;
	///////////////////////////////
	//1�t���[�����ɉ�]����l�����߂�
	//���̌v�Z���ł����ŋ��߂��l���g���̂ŕK���ŏ��ɌĂԂ���
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		void SetOneFrameAngle(const int time_limit);
	//�J�E���g�_�E��UI��\�����鎞�Ԃ��Z�b�g
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		void SetCountDownAngle(const int countdown);
	//�m���}�j�̉�]�l���Z�b�g
	//�߂�l�͉�]�l
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		float SetNormaAngle(const int norma_time);
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		float SetNormaAngleTitle(const int norma_time);
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void UpDateTime();
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void SetTimeLimit(const int limit);
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void SetCountDownTime(const int time);
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void SetNormaTime(const int time);
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void SetNormaAngleEdit(const float angle);
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void SetTitleTime(const int limit,const int norma);
	//�ʏ�̐j�Ɣw�i�}�e���A�����w��̃A���O���Ɉړ�����
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void SetNeedleAndBG_Material(const float angle);
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE bool GetIsTokeiAnimeEnd() const { return IsTokeiAnimeEnd; }
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE	float GetNowTimeAngle() const { return NowTimeAngle; }
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE	float GetNormaTimeAngle() const { return NormaTimeAngle; }
};
