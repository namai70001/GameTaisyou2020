// Fill out your copyright notice in the Description page of Project Settings.


#include "BreakLand.h"
#include "../WaterSurface/FloatActor.h"

void ABreakLand::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AFloatActor* OtherFloat = Cast<AFloatActor>(OtherActor);
	// �ו��ȊO�͔��肵�Ȃ�
	if (GetIsUse()) return; //���łɉ��Ă��邩��
	if (!OtherFloat) return;

	// �Փ˂����A�N�^�[�����e�̎��������ăS�[���ς݂ł��S�[�����Ă��Ȃ����Ƃɂ���
	if (OtherFloat->ActorHasTag("Bom"))
	{
		Break();
		OtherFloat->Destroy();
	}
}

void ABreakLand::Break()
{
	SetIsUse(false);
}