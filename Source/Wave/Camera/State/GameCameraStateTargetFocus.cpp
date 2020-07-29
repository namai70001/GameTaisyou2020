// Fill out your copyright notice in the Description page of Project Settings.

#include "GameCameraStateTargetFocus.h"
#include "../GameCameraActor.h"

void GameCameraStateTargetFocus::OnStart(AGameCameraActor * Actor)
{
}

void GameCameraStateTargetFocus::OnUpdate(AGameCameraActor * Actor)
{
	// �Ȃ߂炩�ɒǏ]
	Actor->SetActorLocation(FMath::Lerp<FVector>(Actor->GetActorLocation(), FocusPoint, 0.1f));
	// �J�����̋������Z�b�g
	Actor->GameCameraBoom->SetTargetDistance(Actor->CharacterDistance_Far);

	Actor->GameCameraBoom->InputUpdateRotation();
}
