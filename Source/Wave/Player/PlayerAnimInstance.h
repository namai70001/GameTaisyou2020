// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class WAVE_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		float Speed;
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		bool IsAttackAnime;

private:
	UAnimMontage* AnimMontage;
public:
	// �R���X�g���N�^
	UPlayerAnimInstance(const FObjectInitializer& ObjectInitializer);

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

	void HummerChergeEvent();
	void HummerAttackEvent();

	UFUNCTION(BlueprintCallable, Category = "C++Library")
	FORCEINLINE bool GetIsAttackAnime() const { return IsAttackAnime; }

};
