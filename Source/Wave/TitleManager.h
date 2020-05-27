// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TitleManager.generated.h"
enum class ETitleState
{
	Title,
	TitleMove,
	StageSelect,
};
class ATitleCamera;
class ATitlePlayer;
class UTitleUI;
class UFadeUI;
UCLASS()
class WAVE_API ATitleManager : public APawn
{
	GENERATED_BODY()
private:
	//�J�����ւ̃A�h���X
	UPROPERTY(EditAnywhere) 
		AActor* TitleCamera = nullptr;
	UPROPERTY(EditAnywhere)
		AActor* TitleMoveCamera = nullptr;
	UPROPERTY(EditAnywhere)
		ATitleCamera* StageSelectCamera = nullptr;
	UPROPERTY(EditAnywhere)
		ATitlePlayer* TitlePlayer = nullptr;
	UPROPERTY(EditAnywhere)
		TSubclassOf<UFadeUI> FadeUIClass;
	UPROPERTY(EditAnywhere)
		TSubclassOf<UTitleUI> TitleUIClass = nullptr;
	UPROPERTY(EditAnywhere, Category = "Instanced")
		float TitleMoveTime = 1.0f;//�^�C�g����ʂ���Z���N�g��ʒ��p�J�����ւ̎���
	UPROPERTY(EditAnywhere, Category = "Instanced")
		float StageSelectTime = 1.0f;//�Z���N�g��ʒ��p����Z���N�g��ʂւ̎���
	ETitleState State;
	int MoveFrameTime;//�J�����ړ����Ԃ��t���[�����Ԃɕϊ�
	int MoveFrameCount;
	bool IsNoInput;//true���L�[���͂𖳌��ɂ���
	UTitleUI* TitleUI;
private:
	void TitleInput();
	//�ړ���̃J�����ƈڍs�X�s�[�h��ݒ�
	void SetCameraMove(AActor* camera, const float camera_speed);
	//�^�C�g���Ń{�^�����������Ƃ��X�e�[�W�Z���N�g�܂ł̃J�����J�ڂ��s��
	void CameraMoveCheck();
public:
	// Sets default values for this pawn's properties
	ATitleManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
