// Fill out your copyright notice in the Description page of Project Settings.


#include "SquareLand.h"
#include "Runtime/Engine/Classes/Kismet//GameplayStatics.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "../MyFunc.h"

void ASquareLand::DebugDraw()
{
	float XOffset = XLength * 0.5f;
	float YOffset = YLength * 0.5f;
	// �f�o�b�O�p�̎l�p��`��(�����⑾���͌��ߑł��j
	UKismetSystemLibrary::DrawDebugLine(this, GetActorLocation() + FVector( XOffset,  YOffset, 0.0f), GetActorLocation() + FVector(-XOffset,  YOffset, 0.0f), LAND_TYPE_COLORS[GetLandTypeNumber()], 0.0f, 3.0f);
	UKismetSystemLibrary::DrawDebugLine(this, GetActorLocation() + FVector( XOffset, -YOffset, 0.0f), GetActorLocation() + FVector(-XOffset, -YOffset, 0.0f), LAND_TYPE_COLORS[GetLandTypeNumber()], 0.0f, 3.0f);
	UKismetSystemLibrary::DrawDebugLine(this, GetActorLocation() + FVector( XOffset,  YOffset, 0.0f), GetActorLocation() + FVector( XOffset, -YOffset, 0.0f), LAND_TYPE_COLORS[GetLandTypeNumber()], 0.0f, 3.0f);
	UKismetSystemLibrary::DrawDebugLine(this, GetActorLocation() + FVector(-XOffset,  YOffset, 0.0f), GetActorLocation() + FVector(-XOffset, -YOffset, 0.0f), LAND_TYPE_COLORS[GetLandTypeNumber()], 0.0f, 3.0f);
}

bool ASquareLand::OnGround(const FVector & Pos)
{
	FVector2D SquarePos = FVector2D(GetActorLocation().X, GetActorLocation().Y);
	float XLen = GetXLength() * 0.5f;
	float YLen = GetYLength() * 0.5f;

	return  SquarePos.X - XLen < Pos.X &&
			SquarePos.X + XLen > Pos.X &&
			SquarePos.Y - YLen < Pos.Y &&
			SquarePos.Y + YLen > Pos.Y;
}

bool ASquareLand::OnGround(const FVector & Pos, float CircleRadius)
{
	FVector2D SquarePos = FVector2D(GetActorLocation().X, GetActorLocation().Y);
	float XLen = GetXLength() * 0.5f;
	float YLen = GetYLength() * 0.5f;

	return  SquarePos.X - XLen < Pos.X - CircleRadius &&
			SquarePos.X + XLen > Pos.X + CircleRadius &&
			SquarePos.Y - YLen < Pos.Y - CircleRadius &&
			SquarePos.Y + YLen > Pos.Y + CircleRadius;
}

bool ASquareLand::InGround(const FVector & Pos, float CircleRadius)
{
	FVector2D SquarePos = FVector2D(GetActorLocation().X, GetActorLocation().Y);
	float XLen = GetXLength() * 0.5f;
	float YLen = GetYLength() * 0.5f;

	return  SquarePos.X - XLen < Pos.X - CircleRadius &&
			SquarePos.X + XLen > Pos.X + CircleRadius &&
			SquarePos.Y - YLen < Pos.Y - CircleRadius &&
			SquarePos.Y + YLen > Pos.Y + CircleRadius;
}

FVector ASquareLand::AdjustMoveInLand(const FVector & Pos, float CircleRadius)
{
	FVector Result = Pos;
	FVector SquareCenter = GetActorLocation();
	float X_HalfLen = GetXLength() * 0.5f;
	float Y_HalfLen = GetYLength() * 0.5f;
	// X�����m�F
	float Deff = Result.X +CircleRadius - (SquareCenter.X + X_HalfLen);
	if (Deff > 0.0f) Result.X -= Deff;
	else
	{
		Deff = Result.X - CircleRadius - (SquareCenter.X - X_HalfLen);
		if (Deff < 0.0f) Result.X -= Deff;
	}
	// Y�����m�F
	Deff = Result.Y + CircleRadius - (SquareCenter.Y + Y_HalfLen);
	if (Deff > 0.0f) Result.Y -= Deff;
	else
	{
		Deff = Result.Y - CircleRadius - (SquareCenter.Y - Y_HalfLen);
		if (Deff < 0.0f) Result.Y -= Deff;
	}

	return Result;
}

FVector ASquareLand::AdjustMoveOutWater(const FVector & OldPos, FVector MovedPos, FVector & MoveVec, float CircleRadius)
{
	FVector2D SquarePos = FVector2D(GetActorLocation().X, GetActorLocation().Y);
	float XLen = GetXLength() * 0.5f;
	float YLen = GetYLength() * 0.5f;

	float XDistance = (SquarePos.X - MovedPos.X);
	float YDistance = (SquarePos.Y - MovedPos.Y);
	float JudgXDistance = CircleRadius + XLen;
	float JudgYDistance = CircleRadius + YLen;

	float X_Deff = JudgXDistance - fabs(XDistance);
	float Y_Deff = JudgYDistance - fabs(YDistance);

	if (X_Deff < 0 || Y_Deff < 0) return MovedPos;

	// �Փ˂��Ă���

	FRay2DCastInfo Info;
	FVector2D VecA, VecB, VecC, VecD;
	// ���C�̌�_�𒲂ׂċ������擾����֐�
	VecA = SquarePos + FVector2D(XLen, YLen);
	VecB = SquarePos + FVector2D(-XLen, YLen);
	VecC = SquarePos + FVector2D(-XLen, -YLen);
	VecD = SquarePos + FVector2D(XLen, -YLen);

	FRay2D PosToMovedPos = FRay2D(OldPos, MovedPos - OldPos);
	MyFunc::Check_CircleRay2D_VS_Ray2D(Info, PosToMovedPos, CircleRadius, FRay2D(VecA, VecB - VecA), FVector2D(0.0f, 1.0f));
	MyFunc::Check_CircleRay2D_VS_Ray2D(Info, PosToMovedPos, CircleRadius, FRay2D(VecB, VecC - VecB), FVector2D(-1.0f, 0.0f));
	MyFunc::Check_CircleRay2D_VS_Ray2D(Info, PosToMovedPos, CircleRadius, FRay2D(VecC, VecD - VecC), FVector2D(0.0f, -1.0f));
	MyFunc::Check_CircleRay2D_VS_Ray2D(Info, PosToMovedPos, CircleRadius, FRay2D(VecD, VecA - VecD), FVector2D(1.0f, 0.0f));

	// �ӂɏՓ˂��Ă�����
	if (Info.IsHit)
	{
		// �Փ˓_�Ɉʒu���C��
		MovedPos = FVector(Info.NearPos, MovedPos.Z);
		// ���˃x�N�g�����Z�o
		FVector2D Ref = (PosToMovedPos.Direction - 2.0f * FVector2D::DotProduct(PosToMovedPos.Direction, Info.NearNormal) * Info.NearNormal);
		MoveVec = FVector(Ref, 0.0f);
	}
	// ���ɖ��܂��Ă�����
	else
	{
		// X���̉����o���i�Ȃ��Ƃ͎v�����ǈꉞ���܂�h�~�p�j
		MovedPos.X += (XDistance > 0) ? -X_Deff : X_Deff;
	}

	return MovedPos;
}

FVector ASquareLand::AdjustMoveOutWater(const FVector & OldPos, FVector MovedPos, FVector & MoveVec, float XLen, float YLen)
{
	FVector2D MyPos = (FVector2D)GetActorLocation();
	float MyXLen = GetXLength() * 0.5f;
	float MyYLen = GetXLength() * 0.5f;
	XLen = XLen * 0.5f;
	XLen = XLen * 0.5f;
	float X01 = MovedPos.X + XLen - MyPos.X - MyXLen;
	float X02 = MyPos.X + MyXLen - MovedPos.X - XLen;

	float Y01 = MovedPos.Y + YLen - MyPos.Y - MyYLen;
	float Y02 = MyPos.Y + MyYLen - MovedPos.Y - YLen;

	if (!((X01 > 0 && X02 > 0) && (Y01 > 0 && Y02 > 0)))
		return MovedPos;

	if (X01 >= X02)
	{

	}

	return MovedPos;
}

// Called when the game starts or when spawned
void ASquareLand::BeginPlay()
{
	Super::BeginPlay();
	Obb.Position = GetActorLocation();
	Obb.HorizontalVector = FVector::RightVector * XLength * 0.5f;
	Obb.VerticalVector = FVector::ForwardVector * YLength * 0.5f;
}