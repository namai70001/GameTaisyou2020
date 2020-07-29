// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"
#include "GameFramework/Actor.h"
#include "Math/Vector.h"
#include "UObject/ConstructorHelpers.h"
#include "PlayerCharacter.h"
#include "Animation/AnimNode_StateMachine.h"

UPlayerAnimInstance::UPlayerAnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UPlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	Speed = 0.0f;
	IsCharge = false;
	IsAttack = false;
	IsDeth = false;
	IsClear = false;
	IsGameOver = false;

	// �X�e�[�g�}�V�����擾
	AnimState = GetStateMachineInstanceFromName(FName("Cat State Machine"));
}

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	APlayerCharacter * OwnerPlayer = Cast<APlayerCharacter>(GetOwningActor());

	//���s���ł���΃A�j���[�V�����Q�Ɛ�̈ړ��X�s�[�h���󂯎��
	if (OwnerPlayer)
	{
		// 0.0f �` 100.0f�̒l�ɂ���
		Speed = OwnerPlayer->GetMoveAmount() * 100.0f;
	}

	// �ʏ��Ԏ�
	if (IsCurrentStateName("Normal State"))
	{
		IsAttack = false;
	}
}

void UPlayerAnimInstance::NativePostEvaluateAnimation()
{
	Super::NativePostEvaluateAnimation();

}

void UPlayerAnimInstance::NativeUninitializeAnimation()
{
	Super::NativeUninitializeAnimation();

}

bool UPlayerAnimInstance::IsCurrentStateName(FName StateName) const
{
	if (!AnimState)
		return false;
	return AnimState->GetCurrentStateName().IsEqual(StateName);
}

void UPlayerAnimInstance::AttackAnimEnd()
{
	if (AttackEndCallBack.IsBound())
		AttackEndCallBack.Execute();
}