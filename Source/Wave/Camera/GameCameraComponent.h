// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "GameCameraComponent.generated.h"


UCLASS( ClassGroup = (Camera), meta=(BlueprintSpawnableComponent) )
class WAVE_API UGameCameraComponent : public USceneComponent
{
	GENERATED_BODY()

public:	

	UGameCameraComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	class UCameraComponent * FollowCamera;

	float FollowDistance = 0.0f;
	float TargetFollowDistance = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
		float RotationSpeed = 1.5f;

private:

	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "90.0"))
		float VerticalMax = 80.0f;
	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true", ClampMin = "-90.0", ClampMax = "0.0"))
		float VerticalMin = 0.0f;

	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// �J�����̉�]����
	void UpdateRotation();
	// �J�����̊p�x�����������`�F�b�N����֐�
	// CheckQuat : �m�F����p�x
	// OverQuat : �I�[�o�[�������̂݃I�[�o�[�������̊p�x��Ԃ�
	bool CheckVerticalLimit(const FQuat & CheckQuat, FQuat & OutOverQuat);
public:	

	void SetCamera(UCameraComponent * Camera) { FollowCamera = Camera; }
	void SetTargetDistance(float Dist) { TargetFollowDistance = Dist; }

};
