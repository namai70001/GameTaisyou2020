// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

DECLARE_DELEGATE(AttackEndDelegate);

UCLASS()
class WAVE_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		float Speed;

public:
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		bool IsCharge;
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		bool IsAttack;
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		bool IsClear;
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		bool IsDeth;
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		bool IsGameOver;
private:
	//UAnimMontage* AnimMontage;
	FAnimNode_StateMachine * AnimState;
public:
	// �R���X�g���N�^
	UPlayerAnimInstance(const FObjectInitializer& ObjectInitializer);

	// �A�^�b�N�I�����Ƀn���}�[�̐�[�����������Ɏ擾
	AttackEndDelegate AttackEndCallBack;

	// �A�j���[�V�����̏�������
	virtual void NativeInitializeAnimation() override;
	// �A�j���[�V�����̍X�V��
	virtual void NativeUpdateAnimation(float DeltaTime) override;
	// �A�j���[�V�����]����
	virtual void NativePostEvaluateAnimation() override;
	// �A�j���[�V�����̏I������
	virtual void NativeUninitializeAnimation() override;

	// �C�x���g�J�n��
	virtual void NativeBeginPlay() override;

	bool IsCurrentStateName(FName StateName) const;
	bool IsChargeAnim() { return IsCurrentStateName("Charge State"); };


	// �u���[�v�����g���ŃA�^�b�N�I�����Ƀn���}�[�̐�[�����������ɓ����
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		void AttackAnimEnd();
};
