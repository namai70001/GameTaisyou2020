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
	StaticMeshComponent = Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass()));

	IsFall = false;
}

// Called every frame
void AFloatActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ���b�V�����Z�b�g����Ă��Ȃ������牽�����Ȃ�
	if (!StaticMeshComponent) return;
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
		Velocity += MoveVec;
	}
	// �t�B�[���h�̊O�ɏo����
	// �R�ɂ���ꍇ�͗���
	if ((!WaterSurface->IsInField(GetActorLocation() + Velocity)))
	{
		IsFall = true;
		/// �������Z���g�p����
		//StaticMeshComponent->SetCollisionObjectType(ECollisionChannel::ECC_Visibility);
		//StaticMeshComponent->SetSimulatePhysics(true);
		Velocity = Velocity.GetSafeNormal() * 8.0f;
		// ���b��ɃI�u�W�F�N�g���폜
		FTimerManager& timerManager = GetWorld()->GetTimerManager();
		FTimerHandle handle;
		timerManager.SetTimer(handle, this, &AFloatActor::MyDestroy, 3.0f);
		return;
	}

	if (Type == FloatType::Circle) SetActorLocation(WaterSurface->AdjustMoveInWater(this, Velocity, Radius));
	else if (Type == FloatType::Square) SetActorLocation(WaterSurface->AdjustMoveInWater(this, Velocity, XLength, YLength));
	Velocity *= 1.0f - Friction;

	// �g�̍����ɍ��킹��
	FVector CurPos = GetActorLocation();
	float Height = WaterSurface->GetWaveHeight(CurPos);
	if (abs(Height) < WaterSurface->GetMaxWaveHeight())
	{
		Height = FMath::Lerp(CurPos.Z, Height, 0.15f);
		SetActorLocation(FVector(CurPos.X, CurPos.Y, Height));
	}
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

		// ���݂ɉ����o��
		MovedPos += outDirection * landingDistance;

		// ���݂ɗ͂�������
		FVector Power = outDirection * landingDistance;
		MoveVec += Power;
		Velocity -= Power;
	}
	break;
	case FloatType::Square:
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
			FVector2D Ref = MyFunc::GetReflectVector2D(PosToMovedPos.Direction, Info.NearNormal);
			MoveVec = FVector(Ref, 0.0f);
		}
		// ���ɖ��܂��Ă�����
		else
		{
			// X���̉����o���i�Ȃ��Ƃ͎v�����ǈꉞ���܂�h�~�p�j
			MovedPos.X += (XDistance > 0) ? -X_Deff : X_Deff;
		}
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
		FVector2D CirclePos = FVector2D(GetActorLocation().X, GetActorLocation().Y);
		XLen = XLen * 0.5f;
		YLen = YLen * 0.5f;

		float XDistance = (CirclePos.X - MovedPos.X);
		float YDistance = (CirclePos.Y - MovedPos.Y);
		float JudgXDistance = Radius + XLen;
		float JudgYDistance = Radius + YLen;

		float X_Deff = JudgXDistance - fabs(XDistance);
		float Y_Deff = JudgYDistance - fabs(YDistance);

		if (X_Deff < 0 || Y_Deff < 0) return MovedPos;

		// �Փ˂��Ă���

		FRay2DCastInfo Info;
		FVector2D VecA, VecB, VecC, VecD;
		// ���C�̌�_�𒲂ׂċ������擾����֐�
		VecA = CirclePos + FVector2D(XLen, YLen);
		VecB = CirclePos + FVector2D(-XLen, YLen);
		VecC = CirclePos + FVector2D(-XLen, -YLen);
		VecD = CirclePos + FVector2D(XLen, -YLen);

		FRay2D PosToMovedPos = FRay2D(OldPos, MovedPos - OldPos);
		MyFunc::Check_CircleRay2D_VS_Ray2D(Info, PosToMovedPos, Radius, FRay2D(VecA, VecB - VecA), FVector2D(0.0f, 1.0f));
		MyFunc::Check_CircleRay2D_VS_Ray2D(Info, PosToMovedPos, Radius, FRay2D(VecB, VecC - VecB), FVector2D(-1.0f, 0.0f));
		MyFunc::Check_CircleRay2D_VS_Ray2D(Info, PosToMovedPos, Radius, FRay2D(VecC, VecD - VecC), FVector2D(0.0f, -1.0f));
		MyFunc::Check_CircleRay2D_VS_Ray2D(Info, PosToMovedPos, Radius, FRay2D(VecD, VecA - VecD), FVector2D(1.0f, 0.0f));

		// �ӂɏՓ˂��Ă�����
		if (Info.IsHit)
		{
			// �Փ˓_�Ɉʒu���C��
			MovedPos = FVector(Info.NearPos, MovedPos.Z);
			// ���˃x�N�g�����Z�o
			FVector2D Ref = MyFunc::GetReflectVector2D(PosToMovedPos.Direction, Info.NearNormal);
			MoveVec = FVector(Ref, 0.0f);
		}
		// ���ɖ��܂��Ă�����
		else
		{
			// X���̉����o���i�Ȃ��Ƃ͎v�����ǈꉞ���܂�h�~�p�j
			MovedPos.X += (XDistance > 0) ? -X_Deff : X_Deff;
		}
	}
	break;
	case FloatType::Square:
	{
		
	}
	break;
	}

	return MovedPos;
}

void AFloatActor::MyDestroy()
{
	//���g���S�[���ɉ^�ԉו���������Q�[���R���g���[���[�ɃQ�[�����̉ו������������Ƃ�ʒm
	if (ActorHasTag("Nimotu"))
	{
		AGameController* game = Cast<AGameController>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameController::StaticClass()));
		if (game)
		{
			game->MinusGameMaxNimotu();
		}
	}
	this->Destroy();
}
