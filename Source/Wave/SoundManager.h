// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SoundManager.generated.h"

UCLASS()
class WAVE_API ASoundManager : public AActor
{
	GENERATED_BODY()
private:
	UPROPERTY(EditAnywhere)
		TArray <UAudioComponent*> AudioComponent;
	
	UPROPERTY(EditAnywhere, category = "SoundSetList")
	TArray <USoundBase*> SoundList;//�Đ����鉹���̃��X�g
public:	

	ASoundManager();

	virtual void BeginPlay() override;

	//@brief �������Đ�����
	//@param Index [in] �Đ����鉹���̃C���f�b�N�X
	void PlaySound(int Index);

	//@brief �������Đ�����
	//@param Index [in] �Đ����鉹���̃C���f�b�N�X
	void Play3DSound(int Index, UWorld* world, FVector vector);

	//@brief �������~����
	//@param Index [in] ��~���鉹���̃C���f�b�N�X
	void StopSound(int Index);

	//@brief �Đ��\���`�F�b�N���s���A�\�Ȃ特�����Đ�����
	//@param Index [in] �Đ����鉹���̃C���f�b�N�X
	static void SafePlaySound(int Index);

	//@brief ��~�\���`�F�b�N���s���A�\�Ȃ特�����~����
	//@param Index [in] ��~���鉹���̃C���f�b�N�X
	static void SafeStopSound(int Index);

		
	static void SafePlay3DSound(int Index, UWorld* world, FVector vector);

	//@brief �T�E���h�}�l�[�W�����擾����
	//@return �T�E���h�}�l�[�W���ւ̃A�h���X
	static ASoundManager* GetInstance();
};
