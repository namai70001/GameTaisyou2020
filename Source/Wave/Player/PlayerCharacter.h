// Fill out your copyright notice in the Description page of Project Settings.
// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
//generated.h�͈�ԍŌ�ɂ����Ȃ��Ƃ����Ȃ�
#include "PlayerCharacter.generated.h"
class UPauseUI;
class UPlayerAnimInstance;
UCLASS(config = Game)
class APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class AGameCameraActor* FollowCamera;
private:
	// �|�[�YUI�@�G�f�B�^�Ŏw�肷��
	UPROPERTY(EditAnywhere)
		TSubclassOf<UPauseUI> UIClass;
	UPauseUI* PauseUI;
	void PauseInput();
public:
	APlayerCharacter();

	//virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		void BeginPlay_C();

	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE	int GetAttackCount() { return AttackCount; };

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C++Class")
		float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C++Class")
		float BaseLookUpRate;

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
	int AttackCount = 0;

	//���ʂɔg�����Ă�
	void WaterAttack();
	//�n���}�[�c��񐔂��}�C�i�X
	void MinusHammerCount();
public:
	/** Returns CameraBoom subobject **/
//	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
//	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};