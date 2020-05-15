// Fill out your copyright notice in the Description page of Project Settings.


#include "WaterWheel.h"
#include "Kismet/GameplayStatics.h"
#include "../WaterSurface/WaterSurface.h"
#include "Kismet/KismetMathLibrary.h"
#include "GenericPlatform/GenericPlatformMath.h"
// Sets default values
AWaterWheel::AWaterWheel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWaterWheel::BeginPlay()
{
	Super::BeginPlay();
	
	WaterSurface = Cast<AWaterSurface>((UGameplayStatics::GetActorOfClass(GetWorld(), AWaterSurface::StaticClass())));
}

// Called every frame
void AWaterWheel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWaterWheel::CheckReverseWheelRotation(FVector pos)
{
	FVector WavePower = WaterSurface->GetWavePower(pos);
	FVector MoveVec = WavePower * 0.1f;
	//�������g�͖�������
	//����Ă��鎞�͖�������l���傫��
	if (MoveVec.Size() < 2.0f)return;
	IsUpRot = !IsUpRot;
	float rot = UKismetMathLibrary::DegAtan2(MoveVec.X, MoveVec.Y);
	Rot = UKismetMathLibrary::Ease(rot, 0.0f, 0.96f, EEasingFunc::Linear);
}

float AWaterWheel::RotationTimeCheck(FVector pos)
{
	if (Rot != 0.0f)
	{
		RotationTime += 1.0f;
		if (RotationTime > 60.0f)
		{//����̐��l�ɂȂ�܂ł͔��Ε����։��Ȃ�
			CheckReverseWheelRotation(pos);
		}
		if (IsUpRot)
		{
			Rot -= 0.01f;
			if (Rot < 0.0f)
			{
				Rot = 0.0f;
				IsUpRot = false;
			}
			//Rot = UKismetMathLibrary::Ease(firstrot, 0.0f, 0.98f, EEasingFunc::Linear);
			return Rot;
		}
		else
		{
			Rot += 0.01f;
			if (Rot > 0.0f)
			{
				Rot = 0.0f;
				IsUpRot = true;
			}
			//Rot = UKismetMathLibrary::Ease(firstrot, 0.0f, 0.98f, EEasingFunc::Linear);
			return Rot;
		}
	}
	IsRotation = false;
	return CheckFirstWheelRotation(pos);
}

float AWaterWheel::CheckFirstWheelRotation(FVector pos)
{

	FVector WavePower = WaterSurface->GetWavePower(pos);
	FVector MoveVec = WavePower * 0.1f;
	//�������g�͖�������
	//����Ă��Ȃ��Ƃ��͖�������덷�͏��Ȃ�
	if (MoveVec.Size() < 1.0f)
	{
		return 0.0f;
	}
	float rot = UKismetMathLibrary::DegAtan2(MoveVec.X, MoveVec.Y);
	//float rot = UKismetMathLibrary::Atan2(MoveVec.X, MoveVec.Y);
	//���̂܂܂��Ɖ�]�ʂ���������̂Œl��������Ό��炷
	if (rot > 0.0f)
	{
		//if (RotationTime <= 0.0f)
		//{
		//	RotationTime = 300.0f;
		//}
		IsUpRot = true;
		//rot = 1.0f;
	}
	else
	{
		//if (RotationTime <= 0.0f)
		//{
		//	RotationTime = 300.0f;
		//}
		IsUpRot = false;
		//rot = -1.0f;
	}
//	Rot = rot;
	Rot = UKismetMathLibrary::Ease(rot, 0.0f, 0.96f, EEasingFunc::Linear);
	firstrot = rot;
	IsRotation = true;
	RotationTime = 0.0f;
	return Rot;
}