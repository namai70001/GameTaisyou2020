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
		float GaugeHP;
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		float MaxChargePowerMax;//�v���C���[���`���[�W�ł���ő��
	//�v���C���[�N���A����HP
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		float ClearHammerHP;
	//�Q�[�W�㏸�A�j�����I�������True�i���U���g�Ŏg�p�j
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		bool IsGaugeAnimeEnd = false;
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		float DefaultHpSizeX;
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		float DefaultHpSizeY;
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		bool IsDamage = false;
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		bool IsDamageDown = false;//�_���[�W�Q�[�W�������Ă��鎞true
public:
	//�n���}�[���\�����J�n��ʒm�C�x���g
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void FirstEvent();
	//���݂�HP���󂯎�茸���Ă�Q�[�W��\������
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void UpdateGauge(const float HammerHP);
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void UpdateDamageGauge(const float HammerHP);
	//���݂�HP���󂯎�茸���Ă�Q�[�W��\������(�|�[�Y�̎��͐ԃQ�[�W��\�����Ȃ��悤�ɂ���
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void UpdatePauseGauge(const float HammerHP);
	//�N�[���^�C�����Ԃ��󂯎��A�C�R���ɔ��f������
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void UpdateCoolTime(const float time);
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
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		void SetMaxChargePowerMax(const float charge) { MaxChargePowerMax = charge; }
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE bool GetIsGaugeAnimeEnd() const { return IsGaugeAnimeEnd; }
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE	float GetGaugeHP() const { return GaugeHP; };
};
