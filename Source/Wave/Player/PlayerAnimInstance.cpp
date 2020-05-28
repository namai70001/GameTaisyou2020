// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"
#include "GameFramework/Actor.h"
#include "Math/Vector.h"
#include "UObject/ConstructorHelpers.h"

UPlayerAnimInstance::UPlayerAnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UPlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	Speed = 0.0f;
	IsCharge = false;
	IsAttackAnime = false;
}

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
	//���s���ł���΃A�j���[�V�����Q�Ɛ�̈ړ��X�s�[�h���󂯎��
	if (TryGetPawnOwner())
	{
		FVector vec_speed = TryGetPawnOwner()->GetVelocity();
		//FVector�̒������擾
		vec_speed.FVector::ToDirectionAndLength(vec_speed, Speed);
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

void UPlayerAnimInstance::HummerChergeEvent()
{
	//�A�j�������^�[�W�����Q��
	//if (!AnimMontage)
	//{
	//	const FName AnimMontageAssetPath(TEXT("AnimMontage'/Game/Main/Player/HummerAttackMontage.HummerAttackMontage'"));
	//	AnimMontage = Cast<UAnimMontage>(StaticLoadObject(UObject::StaticClass(), nullptr, *AnimMontageAssetPath.ToString()));
	//}
	//Montage_Play(AnimMontage, 0.4f);
	//Montage_JumpToSection("Charge", AnimMontage);
	IsCharge = true;
	//Montage_Pause(AnimMontage);
}

void UPlayerAnimInstance::HummerAttackEvent()
{
	IsCharge = false;
	IsAttackAnime = true;
	//Montage_Play(AnimMontage, 0.4f);
	//Montage_JumpToSection("Attack", AnimMontage);
}

void UPlayerAnimInstance::AttackAnimEnd()
{
	IsAttackAnime = false;

	if (AttackEndCallBack.IsBound())
		AttackEndCallBack.Execute();
}
