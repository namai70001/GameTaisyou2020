// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
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
public:	

	ASoundManager();

	virtual void BeginPlay() override;

	//@brief �������Đ�����
	//@param Index [in] �Đ����鉹���̃C���f�b�N�X
	void PlaySound(TCHAR* fileName);

	//@brief �Đ��\���`�F�b�N���s���A�\�Ȃ特�����Đ�����
	//@param Index [in] �Đ����鉹���̃C���f�b�N�X
	static void SafePlaySound(TCHAR* fileName);

	//�w�肵������(Cue)�������Ă���UAudioComponent��Ԃ�
	static UAudioComponent* CreateAudioComponent(TCHAR* fileName);

	//@brief �T�E���h�}�l�[�W�����擾����
	//@return �T�E���h�}�l�[�W���ւ̃A�h���X
	static ASoundManager* GetInstance();



};
