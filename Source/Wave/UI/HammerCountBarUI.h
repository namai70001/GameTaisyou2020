// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HammerCountBarUI.generated.h"

/**
 * 
 */
UCLASS()
class WAVE_API UHammerCountBarUI : public UUserWidget
{
	GENERATED_BODY()
private:
	float NowHp;//���X�ɃQ�[�W�����炷���߂ɍ���HP�Q�[�W�ƌ���O��HP�̍������i�[����
protected:
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		float MaxHammerHP;
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		float DefaultHpSizeX;
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		float DefaultHpSizeY;
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		bool IsDamage;
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		bool IsDamageDown;//�_���[�W�Q�[�W�������Ă��鎞true
public:
	//�n���}�[���\�����J�n��ʒm�C�x���g
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void FirstEvent();
	//���݂�HP���󂯎�茸���Ă�Q�[�W��\������
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void UpdateGauge(const float HammerHP);
	//���݂�HP���󂯎�茸���Ă�Q�[�W��\������(�|�[�Y�̎��͐ԃQ�[�W��\�����Ȃ��悤�ɂ���
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void UpdatePauseGauge(const float HammerHP);
	//�_���[�W�Q�[�W�����炷�̂��J�n����C�x���g
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void ReflectionGauge();
	//���t���[�����炷�Q�[�W�ʂ��v�Z���Ԃ�
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		float DownGauge(const float DamageX,const float HpX);
	//�m���}�o�[�̈ʒu���Z�b�g
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void SetNormaPercent(const float Percent);
	UFUNCTION(BlueprintCallable, Category = "C++Library")
	void SetMaxHammerHP(const float hp) { MaxHammerHP = hp; }
};
