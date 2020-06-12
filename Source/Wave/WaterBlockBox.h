// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "WaterBlockBox.generated.h"
class APlayerCharacter;
/**
 * �v���C���[�����ɗ������������̊O�ɗ����Ȃ��悤�ɂ���Box
 */
UCLASS()
class WAVE_API AWaterBlockBox : public ATriggerBox
{
	GENERATED_BODY()
private:
	APlayerCharacter* GetPlayer;//�v���C���[���
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
public:
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void SetNoCollision();
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void SetONCollision();
};
