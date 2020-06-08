// Fill out your copyright notice in the Description page of Project Settings.
#include "Raft.h"
#include "WaterSurface.h"
#include "Runtime/Engine/Classes/Kismet//GameplayStatics.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "../SoundManager.h"

void ARaft::DebugDraw()
{
	float XOffset = XLength * 0.5f;
	float YOffset = YLength * 0.5f;
	float ZOffset = 50.0f;
	// �f�o�b�O�p�̎l�p��`��(�����⑾���͌��ߑł��j
	UKismetSystemLibrary::DrawDebugLine(this, GetActorLocation() + FVector(XOffset, YOffset, ZOffset), GetActorLocation() + FVector(-XOffset, YOffset, ZOffset), FLinearColor::Green, 0.0f, 5.0f);
	UKismetSystemLibrary::DrawDebugLine(this, GetActorLocation() + FVector(XOffset, -YOffset, ZOffset), GetActorLocation() + FVector(-XOffset, -YOffset, ZOffset), FLinearColor::Green, 0.0f, 5.0f);
	UKismetSystemLibrary::DrawDebugLine(this, GetActorLocation() + FVector(XOffset, YOffset, ZOffset), GetActorLocation() + FVector(XOffset, -YOffset, ZOffset), FLinearColor::Green, 0.0f, 5.0f);
	UKismetSystemLibrary::DrawDebugLine(this, GetActorLocation() + FVector(-XOffset, YOffset, ZOffset), GetActorLocation() + FVector(-XOffset, -YOffset, ZOffset), FLinearColor::Green, 0.0f, 5.0f);
}

void ARaft::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsFloating)
	{
		if (!AudioComponent) AudioComponent = ASoundManager::CreateAudioComponent(SOUND_TYPE::MOVE_RAFT);
		if (!AudioComponent->IsPlaying())AudioComponent->Play();
	}
	else
	{
		if (AudioComponent) AudioComponent->Stop();
	}
}

FVector ARaft::AdjustMoveOnRaft(const FVector & actorPos, const FVector & moveVec, float CircleRadius)
{
	FVector Result = actorPos + moveVec;
	FVector SquareCenter = GetActorLocation();
	float X_HalfLen = XLength * 0.5f;
	float Y_HalfLen = YLength * 0.5f;
	// X�����m�F
	float Deff = Result.X + CircleRadius - (SquareCenter.X + X_HalfLen);
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

bool ARaft::IsInRaft(const FVector & worldPos, float CircleRadius)
{
	FVector2D SquarePos = FVector2D(GetActorLocation());
	float XLen = XLength * 0.5f;
	float YLen = YLength * 0.5f;

	return  SquarePos.X - XLen < worldPos.X - CircleRadius &&
			SquarePos.X + XLen > worldPos.X + CircleRadius &&
			SquarePos.Y - YLen < worldPos.Y - CircleRadius &&
			SquarePos.Y + YLen > worldPos.Y + CircleRadius;
}

bool ARaft::IsOnRaft(FVector worldPos)
{
	float xp = worldPos.X;
	float yp = worldPos.Y;
	float xs = GetActorLocation().X;
	float ys = GetActorLocation().Y;

	if (xp > xs + XLength * 0.5f) return false;
	if (xp < xs - XLength * 0.5f) return false;
	if (yp > ys + YLength * 0.5f) return false;
	if (yp < ys - YLength * 0.5f) return false;

	return true;
}