// Fill out your copyright notice in the Description page of Project Settings.
// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Niagara/Classes/NiagaraSystem.h"	// �G�t�F�N�g�p
//generated.h�͈�ԍŌ�ɂ����Ȃ��Ƃ����Ȃ�
#include "PlayerCharacter.generated.h"
class UPauseUI;
class UPlayerAnimInstance;
class UHammerCountBarUI;
class AWaterSurface;
class ARaft;

UCLASS(config = Game)
class APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = Camera)
		class AGameCameraActor* FollowCamera;
	
private:
	// �|�[�YUI�@�G�f�B�^�Ŏw�肷��
	UPROPERTY(EditAnywhere)
		TSubclassOf<UPauseUI> PauseUIClass;
	UPROPERTY(EditAnywhere)
		TSubclassOf<UHammerCountBarUI> HammerCountBarUIClass;
	UPauseUI* PauseUI = nullptr;
	UHammerCountBarUI* HammerCountBarUI = nullptr;
	//�Q�[�W�ŕ\������p�̃v���C���[�̃n���}�[MAXHP
	UPROPERTY(EditAnywhere, Category = "Instanced")
		float MaxHammerHP = 100.0f;
	//1�t���[�����ɗ��߂�n���}�[�p���[
	UPROPERTY(EditAnywhere, Category = "Instanced")
		float ChargePower = 0.1f;
	float HammerHP;
	void PauseInput();

	FVector PrevPos;

	AWaterSurface* Water;

	ARaft* CurrentRaft;
	bool IsRaftRiding;
public:
	APlayerCharacter();

	//virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		void BeginPlay_C();

	virtual void Tick(float DeltaTime) override;

	// �n���}�[�̐�[�̃g�����X�t�H�[��
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "C++Class")
		USceneComponent* HummerTipPoint = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C++Class")
		float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C++Class")
		float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
		UNiagaraSystem* AttackEffect = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
		float AttackEffectScale = 1.0f;

protected:

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);
	void TurnAtRate(float Rate);

	void LookUpAtRate(float Rate);
	//�L�[���������Ƃ��̃n���}�[�U��
	void TriggerHammerAttack(void);
	//�L�[�𗣂����Ƃ��̃n���}�[�U��
	void ReleaseHammerAttack(void);

	void HummerAttackEnd();
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	//BlueprintReadWrite=Get��Set�֐����u���[�u�����g�Ō��J

	//�n���}�[�U���Ńz�[���h���Ȃ�true
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		bool IsAttackHold;
	//�U���A�j���Đ����Ȃ�true
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		bool IsPlayAttackAnime;
	//�n���}�[�p���[�ϐ�
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		float HammerPower;
private:

	UPlayerAnimInstance* AnimInst;

	//���ʂɔg�����Ă�
	void WaterAttack(FVector Point, float Power);
	//�n���}�[�c��񐔂��}�C�i�X
	void MinusHammerCount();
	//�n���}�[����Q�[�W���}�C�i�X
	void MinusHammerGauge(const float Power);

	bool IsRide;
public:
	/** Returns CameraBoom subobject **/
//	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
//	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};