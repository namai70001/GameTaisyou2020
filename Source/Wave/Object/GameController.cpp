// Fill out your copyright notice in the Description page of Project Settings.

#include "GameController.h"
#include "Kismet/GameplayStatics.h"
#include "Goal.h"
#include "Blueprint/UserWidget.h"
#include "../UI/TimeCountUI.h"
#include "../UI/GameOverUI.h"
#include "../UI/ResultUI.h"
#include "../InputManager.h"
#include "../Player/PlayerCharacter.h"
#include "../WaterSurface/FloatActor.h"
#include "TimerManager.h"
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
	GetPlayer = Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));
	IsGameClear = false;
	IsGameOver = false;
	DataTableLoad();
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
	//MaxNimotu��0�̎��̓f�o�b�O���[�h�Ȃ̂Ō��݂̃}�b�v�̉ו����𓾂�
	if (MaxNimotu == 0)
	{
		GetMaxNimotu();
	}
	GameMaxNimotu = MaxNimotu;
	NotExplotionCount = GetNotExplotionCount(); //���Ă��Ȃ��Ƃ̐�Get
}

// Called every frame
void AGameController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//�v���C���[�ւ̃A�h���X���擾�o���Ă��Ȃ�������ĂіႤ
	if (!GetPlayer)
	{
		GetPlayer = Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));
	}
	CheckTimeCount();
	GameClearCheck();
	GameOverCheck();
	InputGameOverUI();
	InputResultUI();
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

void AGameController::CreateTimeCountUI()
{
	if (TimeCountUI)return;
	if (TimeCountUIClass != nullptr)
	{
		TimeCountUI = CreateWidget<UTimeCountUI>(GetWorld(), TimeCountUIClass);
		if (TimeCountUI != nullptr)
		{
			TimeCountUI->AddToViewport();
			TimeCountUI->SetTimeCount(LimitTime);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("TimeCountUIClass : %s"), L"Widget cannot create");
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("TimeCountUIClass : %s"), L"UIClass is nullptr");
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
			GetPlayer->SetNoTick();
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

void AGameController::CreateResultUI()
{
	if (ResultUI)return;
	if (ResultUIClass != nullptr)
	{
		ResultUI = CreateWidget<UResultUI>(GetWorld(), ResultUIClass);
		if (ResultUI != nullptr)
		{
			ResultUI->AddToViewport();
			GetPlayer->HammerCountBarParent();
			ResultUI->SetResultGaugeAnimeCheckEvent(GetPlayer->GetMaxHammerHP(),GetPlayer->GetHammerHP(), NormaPercent);
			//ResultUI->SetResultGaugeAnimeCheckEvent(100.0f, 80.0f, 30.0f);
			ResultUI->SetResultNowNimotuCheckEvent(GoalCount);
			//ResultUI->SetResultNowNimotuCheckEvent(3);
			ResultUI->SetStampAnimeCheckEvent(NormaGoalCount, MaxNimotu);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("ResultUIClass : %s"), L"Widget cannot create");
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ResultUIClass : %s"), L"UIClass is nullptr");
	}
}

void AGameController::InputGameOverUI()
{
	if (!IsGameOver)return;
	//if (!UGameplayStatics::IsGamePaused(GetWorld()))return;
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

void AGameController::InputResultUI()
{
	if (!IsGameClear)return;
	if (!ResultUI)return;
	const AInputManager * inputManager = AInputManager::GetInstance();
	if (!inputManager)return;
	const InputState * input = inputManager->GetState();
	if (input->Up.IsPress)
	{
		ResultUI->BackSelectState();
	}
	if (input->Down.IsPress)
	{
		ResultUI->NextSelectState();
	}
	if (input->Select.IsPress)
	{
		ResultUI->SelectStateAction();
	}
}

void AGameController::SetNorma()
{
}

int AGameController::GetMaxNimotu()
{
	//���łɉו����𐔂��Ă���Ȃ烊�^�[��
	if (MaxNimotu != 0)return MaxNimotu;
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFloatActor::StaticClass(), FoundActors);
	for (auto Actor : FoundActors)
	{
		AFloatActor* act = Cast<AFloatActor>(Actor);
		if (act)
		{
			if (act->ActorHasTag("Nimotu"))
			{
				MaxNimotu++;
			}
		}
	}
	return MaxNimotu;
}

void AGameController::GameClearCheck()
{
	if (IsGameOver)return;
	auto gameclear = [=]
	{
		//�w��̎��Ԍ�Q�[���N���A�ɂ���
		IsGameClear = true;
		FTimerManager& timerManager = GetWorld()->GetTimerManager();
		FTimerHandle handle;
		timerManager.SetTimer(handle, this, &AGameController::GameClear, 2.8f);
	};
	// �Q�[���N���A����
	//�@�m���}�ȏ�ו������Ă��鎞���n���}�[�����Ďc�莞�Ԃ�0�ɂȂ�����
	if (GoalCount >= NormaGoalCount && GetLimitTimeZero())
	{
		gameclear();
	}
	//�A�ו���S�ē����
	else if (GoalCount == MaxNimotu)
	{
		gameclear();
	}
	//�B�Q�[�����̉ו���0�ŃS�[���J�E���g���m���}���傫��������
	else if (GameMaxNimotu == 0 &&  GoalCount >= NormaGoalCount)
	{
		gameclear();
	}
}

void AGameController::GameOverCheck()
{
	if (IsGameClear)return;
	// �Q�[���I�[�o�[����
	//�m���}��1���B���ł��Ȃ��Ȃ�����Q�[���I�[�o�[
	auto gameover = [=] 
	{ 
		IsGameOver = true;
		//�w��̎��Ԍ�Q�[���I�[�o�[�ɂ���
		FTimerManager& timerManager = GetWorld()->GetTimerManager();
		FTimerHandle handle;
		timerManager.SetTimer(handle, this, &AGameController::GameOver,3.0f);
	};
	//�@�m���}�܂ŉו����^��ł��炸�n���}�[�����Ďc�莞�Ԃ�0�ɂȂ�����
	if (GoalCount < NormaGoalCount && GetLimitTimeZero())
	{
		gameover();
	}
	//�A�ו����m���}���B���ł��Ȃ��قǖ����Ȃ�����(�S�[���ɓ������ו��ƍ��킹��)
	else if (GameMaxNimotu + GoalCount < NormaGoalCount)
	{//���̃Q�[���I�[�o�[�̎���Timer���g��Ȃ�
		IsGameOver = true;
		GameOver();
	}
	//�B�S�[�����m���}�̉ו���菭�Ȃ��Ȃ�����
	else if (NotExplotionCount < NormaGoalCount && GoalCount < NormaGoalCount)
	{
		gameover();
	}
}

void AGameController::GameClear()
{
	CreateResultUI();
	if (TimeCountUI)
	{
		TimeCountUI->RemoveFromParent();
	}
}

void AGameController::GameOver()
{
	CreateGameOverUI();
	if (TimeCountUI)
	{
		TimeCountUI->RemoveFromParent();
	}
}

int AGameController::CountGameNimotu()
{
	int num = 0;
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFloatActor::StaticClass(), FoundActors);
	for (auto Actor : FoundActors)
	{
		AFloatActor* act = Cast<AFloatActor>(Actor);
		if (act)
		{
			if (act->ActorHasTag("Nimotu"))
			{
				num++;
			}
		}
	}
	return num;
}

void AGameController::CheckTimeCount()
{

}

bool AGameController::GetLimitTimeZero()
{
	if (TimeCountUI)
	{
		return TimeCountUI->GetIsCountZero();
	}
	return false;
}

void AGameController::SetTimeCountPause()
{
	if (TimeCountUI)
	{
		TimeCountUI->AnimationPause();
	}
}

void AGameController::SetTimeCountRePlay()
{
	if (TimeCountUI)
	{
		TimeCountUI->AnimationRePlay();
	}
}

bool AGameController::GetIsGameOverUI()
{
	if (GameOverUI)
	{
		return true;
	}
	return false;
}