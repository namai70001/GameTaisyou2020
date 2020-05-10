// Fill out your copyright notice in the Description page of Project Settings.

#include "InputManager.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
//���̓N���X�ւ̃C���X�^���X
static AInputManager* InputManagerInstance = nullptr;

AInputManager::AInputManager()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_LastDemotable;	// �X�V�O���[�v����ԍŌ�ɓo�^����
	SetTickableWhenPaused(true);//�|�[�Y���ł�Tick������悤�ɂ���
}

AInputManager::~AInputManager()
{
	InputManagerInstance = nullptr;
}

void AInputManager::BeginPlay()
{
	Super::BeginPlay();
	InputManagerInstance = this;
}

const AInputManager * AInputManager::GetInstance()
{
	//check(InputManagerInstance);
	return InputManagerInstance;
}

void AInputManager::Tick(float DeltaTime)
{
	//�|�[�Y���Ȃ�|�[�Y�{�^�����͏��̂ݍX�V
	if (UGameplayStatics::IsGamePaused(GetWorld()))
	{	
		State.Pause.Refresh();
		State.Up.Refresh();
		State.Down.Refresh();
		State.Select.Refresh();
		return;
	}
	State.Attack.Refresh();

}

void AInputManager::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AInputManager::InputLS_Vertical);
	PlayerInputComponent->BindAxis("MoveRight", this, &AInputManager::InputLS_Horizontal);

	PlayerInputComponent->BindAxis("LookUpRate", this, &AInputManager::InputRS_Vertical);
	PlayerInputComponent->BindAxis("TurnRate", this, &AInputManager::InputRS_Horizontal);

	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AInputManager::InputAttackPress);
	PlayerInputComponent->BindAction("Attack", IE_Released, this, &AInputManager::InputAttackRelease);

	//�|�[�Y�C���v�b�g
	//�|�[�Y���ł��C���v�b�g����������悤��bExecuteWhenPaused��true�ɂ���
	PlayerInputComponent->BindAction("Pause", IE_Pressed, this, &AInputManager::InputPausePress).bExecuteWhenPaused = true;
	PlayerInputComponent->BindAction("Pause", IE_Released, this, &AInputManager::InputPauseRelease).bExecuteWhenPaused = true;
	PlayerInputComponent->BindAction("Up", IE_Pressed, this, &AInputManager::InputUpPress).bExecuteWhenPaused = true;
	PlayerInputComponent->BindAction("Up", IE_Released, this, &AInputManager::InputUpRelease).bExecuteWhenPaused = true;
	PlayerInputComponent->BindAction("Down", IE_Pressed, this, &AInputManager::InputDownPress).bExecuteWhenPaused = true;
	PlayerInputComponent->BindAction("Down", IE_Released, this, &AInputManager::InputDownRelease).bExecuteWhenPaused = true;
	//����{�^���C���v�b�g
	PlayerInputComponent->BindAction("Select", IE_Pressed, this, &AInputManager::InputSelectPress).bExecuteWhenPaused = true;
	PlayerInputComponent->BindAction("Select", IE_Released, this, &AInputManager::InputSelectRelease).bExecuteWhenPaused = true;
}

