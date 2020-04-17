// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Components/AudioComponent.h"
#include "SoundManager.generated.h"

UCLASS()
class WAVE_API ASoundManager : public AActor
{
	GENERATED_BODY()
private:
	UPROPERTY(EditAnywhere)
	UAudioComponent* AudioComponent = nullptr;//�����R���|�[�l���g�N���X

	UPROPERTY(EditAnywhere, category = "SoundSetList")
	TArray <USoundBase*> SoundList;//�Đ����鉹���̃��X�g
public:	

	ASoundManager();

	//@brief �������Đ�����
	//@param Index [in] �Đ����鉹���̃C���f�b�N�X
	void PlaySound(int Index);

	//@brief �Đ��\���`�F�b�N���s���A�\�Ȃ特�����Đ�����
	//@param Index [in] �Đ����鉹���̃C���f�b�N�X
	static void SafePlaySound(int Index);

	//@brief �T�E���h�}�l�[�W�����擾����
	//@return �T�E���h�}�l�[�W���ւ̃A�h���X
	static ASoundManager* GetInstance();
};
