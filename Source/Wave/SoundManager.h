// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SoundManager.generated.h"

enum class SOUND_TYPE
{
	TITLE_BGM,
	WALK_WOOD,
	STAGE_SELECT,
	WALK_GRASS,
	HAMMER_SMALL,
	HAMMER_MEDIUM,
	HAMMER_BIG,
	GOAL,
	EXPLOSION,
	FALL_ACTOR,
	FALL_PLAYER,
	BREAK_LAND,
	MOVE_RAFT,
	WAVE,
	SPAWN_SLIP,
	COUNT_UP,
	MENU_SELECT,
	MENU_DECISION
};

UCLASS()
class WAVE_API ASoundManager : public AActor
{
	GENERATED_BODY()
private:
	UPROPERTY(EditAnywhere)
		TArray <UAudioComponent*> AudioComponent;	

public:	

	ASoundManager();

	virtual void BeginPlay() override;

	//@brief �������Đ�����
	//@param Index [in] �Đ����鉹���̃C���f�b�N�X
	void PlaySound(SOUND_TYPE type);

	//@brief �Đ��\���`�F�b�N���s���A�\�Ȃ特�����Đ�����
	//@param Index [in] �Đ����鉹���̃C���f�b�N�X
	static void SafePlaySound(SOUND_TYPE type);

	//�w�肵������(Cue)�������Ă���UAudioComponent��Ԃ�
	static UAudioComponent* CreateAudioComponent(SOUND_TYPE type);

	//@brief �T�E���h�}�l�[�W�����擾����
	//@return �T�E���h�}�l�[�W���ւ̃A�h���X
	static ASoundManager* GetInstance();
};
