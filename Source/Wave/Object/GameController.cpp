// Fill out your copyright notice in the Description page of Project Settings.

#include "GameController.h"
#include "Kismet/GameplayStatics.h"
#include "Goal.h"
#include "Blueprint/UserWidget.h"
#include "../UI/StageClearUI.h"
#include "../UI/GameOverUI.h"
#include "../InputManager.h"
#include "../Player/PlayerCharacter.h"
// Sets default values
AGameController::AGameController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGameController::BeginPlay()
{
	Super::BeginPlay();
	
	IsGameClear = false;

	// �V�[����̃S�[����S�Ď擾
	TArray<class AActor*> FoundGoals;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGoal::StaticClass(), FoundGoals);
	for (auto Actor : FoundGoals)
	{
		GoalArray.Add(Cast<AGoal>(Actor));
	}
	SetNorma();
	//�|�[�Y���ł�Tick������悤�ɂ���
	SetTickableWhenPaused(true);
}

// Called every frame
void AGameController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	int GoalCount = GetGoalCount();
	int NotExplotionCount = GetNotExplotionCount(); //���Ă��Ȃ��Ƃ̐�Get
	//�ǂ�����Ă���������m���}�B���s�\�̎��Q�[���I�[�o�[�ɂ���
	if (NotExplotionCount < NormaGoalCount)
	{
		IsGameOver = true;
		CreateGameOverUI();
		if (UGameplayStatics::IsGamePaused(GetWorld()))
		{//�Q�[���I�[�o�[��ʂ̓|�[�Y�����ɂ���̂ŃC���v�b�g���I��莟��return����
			InputGameOverUI();
			return;
		}
		
	}

	// �m���}�B���Ȃ�Ƃ肠�����Q�[���N���A
	if (GoalCount >= NormaGoalCount)
	{
		IsGameClear = true;
		CreateStageClearUI();
	}
}

int AGameController::GetGoalCount()
{
	int Count = 0;
	for (auto Goal : GoalArray)
	{
		if (Goal->GetIsGoal())
			Count++;
	}
	return Count;
}

int AGameController::GetNotExplotionCount()
{
	int Count = 0;
	for (auto Goal : GoalArray)
	{
		if (!Goal->GetIsExplotion())
			Count++;
	}
	return Count;
}

void AGameController::CreateStageClearUI()
{
	if (StageClearUI)return;
	if (StageClearUIClass != nullptr)
	{
		StageClearUI = CreateWidget<UStageClearUI>(GetWorld(), StageClearUIClass);
		if (StageClearUI != nullptr)
		{
			StageClearUI->AddToViewport();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("StageClearUIClass : %s"), L"Widget cannot create");
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("StageClearUIClass : %s"), L"UIClass is nullptr");
	}
}

void AGameController::CreateGameOverUI()
{
	if (GameOverUI)return;
	if (GameOverUIClass != nullptr)
	{
		GameOverUI = CreateWidget<UGameOverUI>(GetWorld(), GameOverUIClass);
		if (GameOverUI != nullptr)
		{
			GameOverUI->AddToViewport();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("GameOverUIClass : %s"), L"Widget cannot create");
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GameOverUIClass : %s"), L"UIClass is nullptr");
	}
}

void AGameController::InputGameOverUI()
{
	if (!GameOverUI)return;
	if (GameOverUI->GetIsPlayAnimation())return;
	const AInputManager * inputManager = AInputManager::GetInstance();
	if (!inputManager)return;
	const InputState * input = inputManager->GetState();
	if (input->Up.IsPress)
	{
		GameOverUI->BackSelectState();
	}
	if (input->Down.IsPress)
	{
		GameOverUI->NextSelectState();
	}
	if (input->Select.IsPress)
	{
		GameOverUI->SelectStateAction();
	}
}

void AGameController::SetNorma()
{
	float percent = NormaPercent * 0.01f;
	APlayerCharacter* player;
	player = Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));
	if (player)
	{
		player->SetNormaPercent(percent);
	}
}