// Fill out your copyright notice in the Description page of Project Settings.
// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.h"
//generated.h�͈�ԍŌ�ɂ����Ȃ��Ƃ����Ȃ�
#include "PlayerCharacter.generated.h"

UCLASS(config = Game)
class APlayerCharacter : public ACharacter
{
	GENERATED_BODY()
		/** Camera boom positioning the camera behind the character */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	/** Follow camera */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	//	class USkeletalMesh* Skeltal;
public:
	APlayerCharacter();
	virtual void BeginPlay() override;
	//�֘A�t����X�L�����b�V��
	//virtual void Tick(float DeltaTime) override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		void WaterAttack();
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
public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};