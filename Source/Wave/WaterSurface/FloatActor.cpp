// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatActor.h"
#include "Kismet/GameplayStatics.h"
#include "WaterSurface.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"

#include "CircleLand.h"
#include "SquareLand.h"
#include "../MyFunc.h"
#include "../Object/GameController.h"
#include "../Object/GoalComponent.h"

#include "../SoundManager.h"
#include "../Camera/GameCameraFocusPoint.h"


// Sets default values
AFloatActor::AFloatActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AFloatActor::BeginPlay()
{
	Super::BeginPlay();
	
	WaterSurface = Cast<AWaterSurface>((UGameplayStatics::GetActorOfClass(GetWorld(), AWaterSurface::StaticClass())));

	IsFall = false;
	IsDeth = false;
}

// Called every frame
void AFloatActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!WaterSurface || IsDeth) return;

	if (IsFall)
	{
		Velocity.Z -= 0.5f;
		//SetActorLocation(GetActorLocation() + Velocity);
		// �ړ��Ɖ�]
		FVector RotAxis = FVector::CrossProduct(Velocity, FVector::UpVector);
		FQuat AddRot = MyFunc::FromAxisAngleToQuaternion(RotAxis, -Velocity.Size() * 0.001f).GetNormalized();
		AddRot = AddRot * GetActorRotation().Quaternion();
		SetActorLocationAndRotation(GetActorLocation() + Velocity, AddRot);

		Velocity *= 1.0f - Friction;
		return;
	}// ���ɗ�������

	// �g�̌X���ɉ����Ĉړ�����
	FVector WavePower = WaterSurface->GetWavePower(GetActorLocation());
	FVector MoveVec = WavePower * FloatSpeed * 0.01f;

	if (MoveVec.Size() > MinFloatWavePower)
	{
		IsFloating = true;
		Velocity += MoveVec;
	}
	else
	{
		IsFloating = false;
	}
	// �t�B�[���h�̊O�ɏo����
	// �R�ɂ���ꍇ�͗���
	if ((!WaterSurface->IsInField(GetActorLocation() + Velocity)))
	{
		IsFall = true;
		
		AGameCameraFocusPoint::SpawnFocusPoint(this, GetActorLocation() + Velocity);
		ASoundManager::SafePlaySound(SOUND_TYPE::FALL_ACTOR);

		Velocity = Velocity.GetSafeNormal() * 8.0f;
		// ���b��ɃI�u�W�F�N�g���폜
		FTimerManager& timerManager = GetWorld()->GetTimerManager();
		FTimerHandle handle;
		timerManager.SetTimer(handle, this, &AFloatActor::MyDestroy, 3.0f);
		return;
	}

	if (Type == FloatType::Circle) SetActorLocation(WaterSurface->AdjustMoveInWater(this, Velocity, Radius, Repulsion));
	else if (Type == FloatType::Square) SetActorLocation(WaterSurface->AdjustMoveInWater(this, Velocity, XLength, YLength, Repulsion));
	Velocity *= 1.0f - Friction;

	// �g�̍����ɍ��킹��
	FVector CurPos = GetActorLocation();
	float Height = WaterSurface->GetWaveHeight(CurPos);
	if (abs(Height) < WaterSurface->GetMaxWaveHeight())
	{
		Height = FMath::Lerp(CurPos.Z, Height, 0.15f);
		SetActorLocation(FVector(CurPos.X, CurPos.Y, Height));
	}

	RootComponent->UpdateChildTransforms();
}

FVector AFloatActor::AdjustMove_VS_Circle(const FVector & OldPos, FVector MovedPos, FVector & MoveVec, float CircleRadius)
{
	switch (Type)
	{
	case FloatType::Circle:
	{
		float distance = FVector::Distance(MovedPos, GetActorLocation());
		float judgDistance = CircleRadius + GetRadius();
		float landingDistance = judgDistance - distance;
		if (landingDistance <= 0) return MovedPos;

		FVector outDirection = MovedPos - GetActorLocation();
		outDirection.Z = 0;
		outDirection.Normalize();

		// �����o��
		MovedPos += outDirection * landingDistance;

		// ���݂ɗ͂�������
		FVector Power = outDirection * landingDistance * 0.5f;
		MoveVec += Power;
		Velocity -= Power * Repulsion;
	}
	break;
	case FloatType::Square:
	{
		FVector2D SquarePos = FVector2D(GetActorLocation());
		float XLen = GetXLength() * 0.5f;
		float YLen = GetYLength() * 0.5f;

		FRay2DCastInfo Info;
		FVector2D VecA, VecB, VecC, VecD;
		VecA = (FVector2D)SquarePos + FVector2D(XLen, YLen);
		VecB = (FVector2D)SquarePos + FVector2D(-XLen, YLen);
		VecC = (FVector2D)SquarePos + FVector2D(-XLen, -YLen);
		VecD = (FVector2D)SquarePos + FVector2D(XLen, -YLen);
		MyFunc::Check_Ray2D_VS_Circle(Info, FRay2D(VecA, VecB - VecA), (FVector2D)MovedPos, CircleRadius);
		MyFunc::Check_Ray2D_VS_Circle(Info, FRay2D(VecB, VecC - VecB), (FVector2D)MovedPos, CircleRadius);
		MyFunc::Check_Ray2D_VS_Circle(Info, FRay2D(VecC, VecD - VecC), (FVector2D)MovedPos, CircleRadius);
		MyFunc::Check_Ray2D_VS_Circle(Info, FRay2D(VecD, VecA - VecD), (FVector2D)MovedPos, CircleRadius);
		if (!Info.IsHit)
		{
			// ���S�ɖ��܂��Ă���ꍇ
			if (VecA.X > MovedPos.X && VecC.X < MovedPos.X &&
				VecA.Y > MovedPos.Y && VecC.Y < MovedPos.Y)
				return OldPos;

			return MovedPos;
		}

		// �Փ˂��Ă���
		float PushValue = (CircleRadius - Info.HitDist);
		MovedPos += FVector(-Info.NearNormal * PushValue, 0.0f);
		FVector Ref = FVector(MyFunc::GetReflectVector2D((FVector2D)MoveVec, -Info.NearNormal), 0.0f);
		MoveVec = Ref * 0.5f;
		Velocity -= MoveVec * Repulsion;
	}
	break;
	}
	

	return MovedPos;
}

FVector AFloatActor::AdjustMove_VS_Square(const FVector & OldPos, FVector MovedPos, FVector & MoveVec, float XLen, float YLen)
{
	switch (Type)
	{
	case FloatType::Circle:
	{
		FVector2D CirclePos = FVector2D(GetActorLocation());
		XLen = XLen * 0.5f;
		YLen = YLen * 0.5f;

		FRay2DCastInfo Info;
		FVector2D VecA, VecB, VecC, VecD;
		VecA = (FVector2D)MovedPos + FVector2D(XLen, YLen);
		VecB = (FVector2D)MovedPos + FVector2D(-XLen, YLen);
		VecC = (FVector2D)MovedPos + FVector2D(-XLen, -YLen);
		VecD = (FVector2D)MovedPos + FVector2D(XLen, -YLen);
		MyFunc::Check_Ray2D_VS_Circle(Info, FRay2D(VecA, VecB - VecA), CirclePos, Radius);
		MyFunc::Check_Ray2D_VS_Circle(Info, FRay2D(VecB, VecC - VecB), CirclePos, Radius);
		MyFunc::Check_Ray2D_VS_Circle(Info, FRay2D(VecC, VecD - VecC), CirclePos, Radius);
		MyFunc::Check_Ray2D_VS_Circle(Info, FRay2D(VecD, VecA - VecD), CirclePos, Radius);
		if (!Info.IsHit)
		{
			// ���S�ɖ��܂��Ă���ꍇ
			if (VecA.X > CirclePos.X && VecC.X < CirclePos.X &&
				VecA.Y > CirclePos.Y && VecC.Y < CirclePos.Y)
				return OldPos;

			return MovedPos;
		}
		// �Փ˂��Ă���
		float PushValue = (Radius - Info.HitDist);
		MovedPos += FVector(Info.NearNormal * PushValue, 0.0f);
		MoveVec = FVector(MyFunc::GetReflectVector2D((FVector2D)MoveVec, Info.NearNormal), 0.0f) * 0.5f;
		Velocity -= MoveVec * Repulsion;
	}
	break;
	case FloatType::Square:
	{
		FVector2D MyPos = (FVector2D)GetActorLocation();
		float MyXLen = GetXLength() * 0.5f;
		float MyYLen = GetYLength() * 0.5f;
		XLen = XLen * 0.5f;
		YLen = YLen * 0.5f;
		// ���܂��Ă���ʂ��v�Z
		float X_Left = MovedPos.X + XLen - (MyPos.X - MyXLen);
		float X_Right = MyPos.X + MyXLen - (MovedPos.X - XLen);
		float Y_Down = MovedPos.Y + YLen - (MyPos.Y - MyYLen);
		float Y_Up = MyPos.Y + MyYLen - (MovedPos.Y - YLen);

		if (!((X_Left > 0 && X_Right > 0) && (Y_Down > 0 && Y_Up > 0)))
			return MovedPos;

		// �����o���ʂ��v�Z
		FVector2D PushVec = FVector2D::ZeroVector;
		PushVec.X = (MoveVec.X > 0) ? -X_Left : X_Right;	// true:�����ɂ��鎞, false:�E���ɂ��鎞
		PushVec.Y = (MoveVec.Y > 0) ? -Y_Down : Y_Up;		// true:�����ɂ��鎞, false:�㑤�ɂ��鎞

		// �����o���ʂ��傫�������͖����ɂ���
		(FMath::Abs(PushVec.X) >= FMath::Abs(PushVec.Y)) ? PushVec.X = 0.0f : PushVec.Y = 0.0f;
		MovedPos += FVector(PushVec, 0.0f);

		// ���˕������v�Z
		FVector Ref = FVector(MyFunc::GetReflectVector2D((FVector2D)MoveVec, PushVec.GetSafeNormal()), 0.0f);
		MoveVec = Ref * 0.5f;
		Velocity -= MoveVec * Repulsion;

		return MovedPos;
	}
	break;
	}

	return MovedPos;
}

void AFloatActor::MyDestroy()
{
	AGameController* game = Cast<AGameController>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameController::StaticClass()));
	if (!game) return;

	//���g���S�[���ɉ^�ԉו���������Q�[���R���g���[���[�ɃQ�[�����̉ו������������Ƃ�ʒm
	if (ActorHasTag("Nimotu"))
	{
		game->MinusGameMaxNimotu();
	}
	else
	{
		// �S�[���������Ă����ꍇ�̓S�[�����󂵂����Ƃɂ���
		UGoalComponent * Goal = Cast<UGoalComponent>(GetComponentByClass(UGoalComponent::StaticClass()));
		if (Goal)
			Goal->SetGoalMinus();
	}

	this->Destroy();
}
