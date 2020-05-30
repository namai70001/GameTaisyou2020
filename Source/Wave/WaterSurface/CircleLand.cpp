// Fill out your copyright notice in the Description page of Project Settings.


#include "CircleLand.h"
#include "Runtime/Engine/Classes/Kismet//GameplayStatics.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"

void ACircleLand::DebugDraw()
{
	// �f�o�b�O�p�̃T�[�N����`��(�����⑾���͌��ߑł��j
	UKismetSystemLibrary::DrawDebugCylinder(this, GetActorLocation(), GetActorLocation() - FVector(0,0,10), Radius, 64, LAND_TYPE_COLORS[GetLandTypeNumber()], 0.0f, 3.0f);
}

bool ACircleLand::OnGround(const FVector & Pos)
{
	FVector Dist = GetActorLocation() - Pos;
	return Dist.SizeSquared() < Radius * Radius;
}

FVector ACircleLand::AdjustMoveInLand(const FVector & Pos, float CircleRadius)
{
	FVector Result = Pos;
	// �~�̊O�ɏo�Ȃ��悤�ɂ��鏈���ōs����͂�
	FVector CircleCenter = GetActorLocation();
	FVector MovedPosDir = Pos - CircleCenter;
	float Deff = MovedPosDir.Size() + CircleRadius - GetRadius();
	// �~�̊O�ɂ͂ݏo���ꍇ�݈̂ʒu���ړ�������
	if (Deff > 0)
	{
		MovedPosDir.Normalize();
		Result -= MovedPosDir * Deff;
	}
	return Result;
}
