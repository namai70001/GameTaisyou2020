// Fill out your copyright notice in the Description page of Project Settings.

#include "InputManager.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Engine/World.h"

#define MAX_INPUT_INSTANCE (2)

//���̓N���X�ւ̃C���X�^���X
static AInputManager* InputManagerInstances[MAX_INPUT_INSTANCE] = {};

AInputManager::AInputManager()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_LastDemotable;	// �X�V�O���[�v����ԍŌ�ɓo�^����
	SetTickableWhenPaused(true);//�|�[�Y���ł�Tick������悤�ɂ���
}

AInputManager::~AInputManager()
{
	InputManagerInstances[AutoPossessPlayer - 1] = nullptr;
}

void AInputManager::BeginPlay()
{
	Super::BeginPlay();

	// ��ڂɐ������ꂽ�C���X�^���X���c��̃C���X�^���X�𐶐�����
	// ��ڈȍ~�͊��ɓo�^�ς݂Ȃ̂Ń��^�[��
	if (InputManagerInstances[0]) return;

	// ���̓C�x���g���󂯎��ԍ����w��
	this->AutoPossessPlayer = EAutoReceiveInput::Player0;
	this->AutoReceiveInput = this->AutoPossessPlayer;
	InputManagerInstances[0] = this;
	
	for (int i = 1; i < MAX_INPUT_INSTANCE; i++)
	{
		UGameplayStatics::CreatePlayer(GetWorld(), i);
	}

	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInputManager::StaticClass(), actors);

	int num = 1;
	for (auto actor : actors)
	{
		if (actor == this) continue;

		InputManagerInstances[num] = Cast<AInputManager>(actor);
		num++;
	}

	//for (int i = 1; i < MAX_INPUT_INSTANCE; i++)
	//{
	//	AInputManager * Instance = GetWorld()->SpawnActor<AInputManager>();
	//	// ���̓C�x���g���󂯎��ԍ����w��
	//	Instance->AutoPossessPlayer = (EAutoReceiveInput::Type)(i + 1);
	//	Instance->AutoReceiveInput = Instance->AutoPossessPlayer;
	//	Instance->SetPlayerState(GetPlayerState());
	//	Instance->Controller = UGameplayStatics::CreatePlayer(GetWorld(), i);
	//	if (Instance->Controller)
	//	{
	//		Instance->SetupPlayerInputComponent(Instance->Controller->InputComponent);
	//		InputManagerInstances[i] = Instance;
	//	}
	//}
}

const AInputManager * AInputManager::GetInstance(EAutoReceiveInput::Type Index)
{
	if (MAX_INPUT_INSTANCE < Index)
		return nullptr;

	return InputManagerInstances[Index - 1];
}

void AInputManager::Tick(float DeltaTime)
{
	State.Pause.Refresh();
	State.Up.Refresh();
	State.Down.Refresh();
	State.Select.Refresh();
	State.Back.Refresh();
	State.Right.Refresh();
	State.Left.Refresh();
//�|�[�Y���Ȃ�|�[�Y�{�^�����͏��̂ݍX�V
	if (UGameplayStatics::IsGamePaused(GetWorld()))return;
	State.Attack.Refresh();
	State.AttackCancel.Refresh();
	State.RightStickButton.Refresh();

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

	PlayerInputComponent->BindAction("AttackCancel", IE_Pressed, this, &AInputManager::InputAttackCancelPress);
	PlayerInputComponent->BindAction("AttackCancel", IE_Released, this, &AInputManager::InputAttackCancelRelease);

	PlayerInputComponent->BindAction("ChangeCameraType", IE_Pressed, this, &AInputManager::InputRightStickButtonPress);
	PlayerInputComponent->BindAction("ChangeCameraType", IE_Released, this, &AInputManager::InputRightStickButtonRelease);

	//�|�[�Y�C���v�b�g
	//�|�[�Y���ł��C���v�b�g����������悤��bExecuteWhenPaused��true�ɂ���
	PlayerInputComponent->BindAction("Pause", IE_Pressed, this, &AInputManager::InputPausePress).bExecuteWhenPaused = true;
	PlayerInputComponent->BindAction("Pause", IE_Released, this, &AInputManager::InputPauseRelease).bExecuteWhenPaused = true;
	PlayerInputComponent->BindAction("Up", IE_Pressed, this, &AInputManager::InputUpPress).bExecuteWhenPaused = true;
	PlayerInputComponent->BindAction("Up", IE_Released, this, &AInputManager::InputUpRelease).bExecuteWhenPaused = true;
	PlayerInputComponent->BindAction("Down", IE_Pressed, this, &AInputManager::InputDownPress).bExecuteWhenPaused = true;
	PlayerInputComponent->BindAction("Down", IE_Released, this, &AInputManager::InputDownRelease).bExecuteWhenPaused = true;
	PlayerInputComponent->BindAction("Right", IE_Pressed, this, &AInputManager::InputRightPress).bExecuteWhenPaused = true;
	PlayerInputComponent->BindAction("Right", IE_Released, this, &AInputManager::InputRightRelease).bExecuteWhenPaused = true;
	PlayerInputComponent->BindAction("Left", IE_Pressed, this, &AInputManager::InputLeftPress).bExecuteWhenPaused = true;
	PlayerInputComponent->BindAction("Left", IE_Released, this, &AInputManager::InputLeftRelease).bExecuteWhenPaused = true;
	//����{�^���C���v�b�g
	PlayerInputComponent->BindAction("Select", IE_Pressed, this, &AInputManager::InputSelectPress).bExecuteWhenPaused = true;
	PlayerInputComponent->BindAction("Select", IE_Released, this, &AInputManager::InputSelectRelease).bExecuteWhenPaused = true;
	//�߂�{�^���C���v�b�g
	PlayerInputComponent->BindAction("Back", IE_Pressed, this, &AInputManager::InputBackPress).bExecuteWhenPaused = true;
	PlayerInputComponent->BindAction("Back", IE_Released, this, &AInputManager::InputBackRelease).bExecuteWhenPaused = true;
}

