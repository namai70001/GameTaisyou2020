// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "GameCameraComponent.h"
#include "GameCameraActor.generated.h"

class GameCameraState;

UCLASS()
class WAVE_API AGameCameraActor : public AActor
{
	GENERATED_BODY()

public:	
	enum class FollowType : int
	{
		FieldCenter = 0,
		CharacterFollow_Far,
		CharacterFollow_Near,
		Num
	};

	// Sets default values for this actor's properties
	AGameCameraActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UGameCameraComponent * GameCameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent * Camera;

	// �t�B�[���h�̒��S���W�i�Q�[���X�^�[�g���̃J�����̒��S���W�j
	FVector FieldCenterPos;
	// �J�������Ǐ]����^�[�Q�b�g
	ACharacter * FollowTarget;
	FollowType Type = FollowType::FieldCenter;
	TArray<AActor*> FocusPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
		float FieldDistance = 800.0f; // �X�e�[�W���~�낵���_�ł̃J�����̋���
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
		float CharacterDistance_Far = 500.0f; // �v���C���[���~�낵���_�ł̃J�����̋���
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
		float CharacterDistance_Near = 200.0f; // �v���C���[�Ǐ]���_�ł̃J�����̋���
protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	GameCameraState * State;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ChangeState(GameCameraState * NewState);

	// �C�x���g�F�J�����̃o�C�u���[�V����
	UFUNCTION(BlueprintCallable, Category = "C++Event", BlueprintImplementableEvent)
		void EventCameraShake(float Scale);

	void SetFollowTarget(ACharacter * Target) { FollowTarget = Target; }
	void InputChangeType();
};
