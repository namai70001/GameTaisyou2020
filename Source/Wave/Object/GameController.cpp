// Fill out your copyright notice in the Description page of Project Settings.

#include "GameController.h"
#include "Kismet/GameplayStatics.h"
#include "GoalComponent.h"
#include "Blueprint/UserWidget.h"
#include "../UI/GameTimeUI.h"
#include "../UI/GameOverUI.h"
#include "../UI/ResultUI.h"
#include "../UI/NimotuCountUI.h"
#include "../UI/PauseUI.h"
#include "../UI/ControlTipsUI.h"
#include "../InputManager.h"
#include "../Player/PlayerCharacter.h"
#include "../WaterSurface/FloatActor.h"
#include "TimerManager.h"
#include "../SoundManager.h"
#include "../Camera/GameCameraFocusPoint.h"
#include "../UI/FadeUI.h"
#include "../Camera/GameCameraActor.h"
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
	IsStartResultEvent = false;
	NotExplotionCount = 0;
	DataTableLoad();

	//�|�[�Y���ł�Tick������悤�ɂ���
	SetTickableWhenPaused(true);

	// �V�[����̃S�[����S�Ď擾
	TArray<class AActor*> FoundGoals;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), FoundGoals);
	for (auto Actor : FoundGoals)
	{
		UGoalComponent * Goal = Cast<UGoalComponent>(Actor->GetComponentByClass(UGoalComponent::StaticClass()));
		if (!Goal) continue;

		if(!Goal->GetIsExplotion())
			NotExplotionCount++;
	}

	//MaxNimotu��0�̎��̓f�o�b�O���[�h�Ȃ̂Ō��݂̃}�b�v�̉ו����𓾂�
	if (MaxNimotu == 0)
	{
		GetMaxNimotu();
	}
	GameMaxNimotu = MaxNimotu;
	if (DebugScreenMode)
	{
		GetPlayer->SetPlayerHiddenInGame();
		GetPlayer->DebugHammerCountBarParent();
		GetPlayer->SetNoTick();
		return;
	}
	CreateNimotuCountUI();
	CreateGameTimeUI();
	GetPlayer->CreateHammerCountBarUI();
	if (IsTips)
	{
		CreateControlTipsUI();
	}
	//�Ō�Ƀt�F�[�h�A�E�g���o�����ߍŌ�ɐ���
	InitFadeOut();

	AudioComponent = ASoundManager::CreateAudioComponent(SOUND_TYPE::STAGE_BGM);
	AudioComponent->Play(0.0f);
}

// Called every frame
void AGameController::Tick(float DeltaTime)
{
	if (DebugScreenMode)return;
	Super::Tick(DeltaTime);
	//�v���C���[�ւ̃A�h���X���擾�o���Ă��Ȃ�������ĂіႤ
	if (!GetPlayer)
	{
		GetPlayer = Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));
	}
	////�A�^�b�N�A�j�����̓|�[�Y���J���Ȃ��悤�ɂ���
	//if (!GetPlayer->GetIsAttack() && !GetPlayer->GetIsCharge())
	//{
	////	InputPause();
	//}

	//�J�����̒��ڃ|�C���g�����݂���ꍇ�̓Q�[���̔�������Ȃ�
	if (!IsStartResultEvent && GetPlayer->GetCameraActor()->FocusPoints.Num() == 0)
	{
		//�w��̎��Ԍ�Ɋ֐������s����
		// �Q�[���I�[�o�[��D�悵�Ĕ��肷��i�S�ăS�[����ɉƂ����������ꂽ�肷�邽�߁j
		if (IsGameOver)
		{
			FTimerManager& timerManager = GetWorld()->GetTimerManager();
			FTimerHandle handle;
			timerManager.SetTimer(handle, this, &AGameController::GameOver, 1.0f);
			IsStartResultEvent = true;
		}
		else if (IsGameClear)
		{
			GetPlayer->SetGameClear();
		    ASoundManager::GetInstance()->PlaySound(SOUND_TYPE::CLEAR_BGM);
			FTimerManager& timerManager = GetWorld()->GetTimerManager();
			FTimerHandle handle;
			timerManager.SetTimer(handle, this, &AGameController::GameClear, 2.8f);
			IsStartResultEvent = true;
		}
	}

	GameOverCheck();
	GameClearCheck();

	//InputGameOverUI();
	UpdateTime();
}

void AGameController::CreateGameTimeUI()
{
	if (GameTimeUI)return;
	if (GameTimeUIClass != nullptr)
	{
		GameTimeUI = CreateWidget<UGameTimeUI>(GetWorld(), GameTimeUIClass);
		if (GameTimeUI != nullptr)
		{
			GameTimeUI->AddToViewport();
			GameTimeUI->SetTimeLimit(TimeLimit);
			GameTimeUI->SetCountDownTime(CountDownTime);
			GameTimeUI->SetNormaTime(NormaTime);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("GameTimeUIClass : %s"), L"Widget cannot create");
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GameTimeUIClass : %s"), L"UIClass is nullptr");
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
			//���̃X�e�[�W���N���A�������̐������ԁA�N���A���̐j�A���O���A�m���}�j�A���O���A�m���}���Ԃ��Z�b�g
			ResultUI->SetResultTokeiAnimeCheckEvent(TimeLimit, GetNowTimeAngle(), GetNormaTimeAngle(),NormaTime);
			ResultUI->SetResultNowNimotuCheckEvent(GoalCount);
			ResultUI->SetStampAnimeCheckEvent(NormaGoalCount, MaxNimotu);
			SetTimeCountPause();
			IsResult = true;
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

void AGameController::CreateNimotuCountUI()
{
	if (NimotuCountUI)return;
	if (NimotuCountUIClass != nullptr)
	{
		NimotuCountUI = CreateWidget<UNimotuCountUI>(GetWorld(), NimotuCountUIClass);
		if (NimotuCountUI != nullptr)
		{
			NimotuCountUI->AddToViewport();
			NimotuCountUI->SetStageNimotu(GameMaxNimotu);
			NimotuCountUI->SetStageInNimotu(GoalCount);
			NimotuCountUI->SetNormaNimotu(NormaGoalCount);
			NimotuCountUI->SetMaxNimotu(MaxNimotu);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("NimotuCountUIClass : %s"), L"Widget cannot create");
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("NimotuCountUIClass : %s"), L"UIClass is nullptr");
	}
}

void AGameController::InitFadeOut()
{
	if (FadeUIClass != nullptr)
	{
		UFadeUI* FadeUI = CreateWidget<UFadeUI>(GetWorld(), FadeUIClass);
		if (FadeUI != nullptr)
		{
			FadeUI->AddToViewport();
			FadeUI->SetFade(FLinearColor(0.0f, 0.0f, 0.0f, 1.0f), false, 1.0f, true);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("FadeUIClass : %s"), L"Widget cannot create");
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("FadeUIClass : %s"), L"UIClass is nullptr");
	}
}

void AGameController::InputGameOverUI()
{
	if (!IsGameOver)return;
	if (!GameOverUI)return;
	if (GameOverUI->GetIsPlayAnimation())return;
	const AInputManager * inputManager = AInputManager::GetInstance();
	if (!inputManager)return;
	const InputState * input = inputManager->GetState();
	if (input->Left.IsPress)
	{
		GameOverUI->BackSelectState();
	}
	if (input->Right.IsPress)
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
	if (input->Left.IsPress)
	{
		ResultUI->BackSelectState();
	}
	if (input->Right.IsPress)
	{
		ResultUI->NextSelectState();
	}
	if (input->Select.IsPress)
	{
		ResultUI->SelectStateAction();
	}
}

void AGameController::InputPause()
{
	const AInputManager * inputManager = AInputManager::GetInstance();
	if (!inputManager)return;
	const InputState * input = inputManager->GetState();
	if (input->Pause.IsPress)
	{//�|�[�Y���łȂ���΃|�[�Y��ʂ��J���A�|�[�Y����������|�[�Y��ʂ����
		//�Q�[���I�������𖞂����Ă�����|�[�Y���J���Ȃ��悤�ɂ���
		if (GetIsClear() || GetIsGameOver())return;
		if (!UGameplayStatics::IsGamePaused(GetWorld()))
		{
			if (PauseUIClass != nullptr)
			{//���߂ă|�[�Y��ʂ��J���Ƃ���UI�𐶐�����
				if (!PauseUI)
				{
					PauseUI = CreateWidget<UPauseUI>(GetWorld(), PauseUIClass);
					PauseUI->AddToViewport();
					PauseUI->SetNormaAngle(GetNormaTimeAngle());
					PauseUI->SetNeedleAndBG_Material(GetNowTimeAngle());
					PauseUI->SetTimeLimit(TimeLimit);
					PauseUI->SetNormaTime(TimeLimit - NormaTime);
					SetTimeCountPause();
				}
				else if (PauseUI)
				{
					if (PauseUI->GetIsPlayAnimation())return;
					PauseUI->AddToViewport();
					PauseUI->SetNormaAngle(GetNormaTimeAngle());
					PauseUI->SetNeedleAndBG_Material(GetNowTimeAngle());
					PauseUI->SetTimeLimit(TimeLimit);
					PauseUI->SetNormaTime(TimeLimit - NormaTime);
					SetTimeCountPause();
				}
				//�������Ă�nullptr��������G���[���\��
				if (PauseUI == nullptr)
				{
					UE_LOG(LogTemp, Error, TEXT("PauseUI : %s"), L"Widget cannot create");
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("PauseUI : %s"), L"PauseUIClass is nullptr");
			}
		}
		else if (UGameplayStatics::IsGamePaused(GetWorld()))
		{
			if (!PauseUI)return;
			if (PauseUI->GetIsPlayAnimation())return;
			PauseUI->EndAnimation();
		}
	}
	if (!UGameplayStatics::IsGamePaused(GetWorld()))return;
	if (!PauseUI)return;
	if (input->Left.IsPress)
	{
		PauseUI->BackSelectState();
	}
	if (input->Right.IsPress)
	{
		PauseUI->NextSelectState();
	}
	if (input->Select.IsPress)
	{
		PauseUI->SelectStateAction();
	}
}

void AGameController::CreateControlTipsUI()
{
	if (ControlTipsUI)return;
	if (ControlTipsUIClass != nullptr)
	{
		ControlTipsUI = CreateWidget<UControlTipsUI>(GetWorld(), ControlTipsUIClass);
		if (ControlTipsUI != nullptr)
		{
			ControlTipsUI->AddToViewport();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("ControlTipsUIClass : %s"), L"Widget cannot create");
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ControlTipsUIClass : %s"), L"UIClass is nullptr");
	}
}

void AGameController::UpdateTime()
{
	//�Q�[�����I�����Ă����玞�v��i�߂Ȃ�
	if (GetIsClear() || GetIsGameOver())return;
	if (UGameplayStatics::IsGamePaused(GetWorld()))
	{
		SetTimeCountPause();
		return;
	}
	if (!UGameplayStatics::IsGamePaused(GetWorld()))
	{
		SetTimeCountRePlay();
	}
	if (GameTimeUI)
	{
		GameTimeUI->UpDateTime();
	}
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
	//if (IsGameOver)return;
	if (IsGameClear)return;
	auto gameclear = [=]
	{
		RemoveUI();
		//�w��̎��Ԍ�Q�[���N���A�ɂ���
		IsGameClear = true;
		SetTimeCountPause();
		AudioComponent->FadeOut(1.0f, 0.0f);
	};
	// �Q�[���N���A����
	//�@�m���}�ȏ�ו������Ă��鎞���c�莞�Ԃ�0�ɂȂ�����
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
	//�C�S�[�����S�ē����Ă��邩�ו����c���Ă��邩�m���}�ȏ�ɃS�[���ɓ���Ă�����N���A
	else if (NotExplotionCount - GoalCount <= 0 && GameMaxNimotu > 0 && GoalCount >= NormaGoalCount)
	{
		gameclear();
	}
}

void AGameController::GameOverCheck()
{
	//if (IsGameClear)return;
	if (IsGameOver)return;
	// �Q�[���I�[�o�[����
	//�m���}��1���B���ł��Ȃ��Ȃ�����Q�[���I�[�o�[
	auto gameover = [=]
	{ 
		RemoveUI();
		IsGameOver = true;
		SetTimeCountPause();
		AudioComponent->FadeOut(2.0f, 0.0f);
	};
	//�@�m���}�܂ŉו����^��ł��炸�c�莞�Ԃ�0�ɂȂ�����
	if (GoalCount < NormaGoalCount && GetLimitTimeZero())
	{
		gameover();
	}
	//�A�v���C���[������������
	else if (GetPlayer->GetIsDeth())
	{
		gameover();
	}
	//�B�ו����m���}���B���ł��Ȃ��قǖ����Ȃ�����(�S�[���ɓ������ו��ƍ��킹��)
	else if (GameMaxNimotu + GoalCount < NormaGoalCount)
	{
		gameover();
	}
	//�C�S�[�����m���}�̉ו���菭�Ȃ��Ȃ�����
	else if (NotExplotionCount < NormaGoalCount && GoalCount - NotExplotionCount < NormaGoalCount)
	{
		gameover();
	}
}

void AGameController::GameClear()
{
	CreateResultUI();
}

void AGameController::GameOver()
{
	CreateGameOverUI();
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

bool AGameController::GetLimitTimeZero()
{
	if (GameTimeUI)
	{
		return GameTimeUI->GetIsCountZero();
	}
	return false;
}

void AGameController::SetTimeCountPause()
{
	if (GameTimeUI)
	{
		GameTimeUI->AnimationPause();
	}
}

void AGameController::SetTimeCountRePlay()
{
	if (GameTimeUI)
	{
		GameTimeUI->AnimationRePlay();
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

void AGameController::AddGoalCount()
{
	GoalCount++; 
	//���݃m���}�Ɠ����������Ă�����A�j���[�V�����Đ�
	if (NormaGoalCount == GoalCount)
	{
		NimotuCountUI->NormaInAnimation();
	}
	NimotuCountUI->NimotuInAnimation();
	NimotuCountUI->SetStageInNimotu(GoalCount);
}

void AGameController::MinusGoalCount()
{ 
	//���݃m���}�Ɠ����������Ă�����m���}�ȉ��̉ו��ɂȂ������Ƃ������A�j���[�V�����Đ�
	if (NormaGoalCount == GoalCount)
	{
		NimotuCountUI->NormaNoAnimation();
	}
	GoalCount--;
	NimotuCountUI->NimotuInAnimation();
	NimotuCountUI->SetStageInNimotu(GoalCount);
}

void AGameController::MinusGameMaxNimotu()
{ 
	GameMaxNimotu--;
	NimotuCountUI->SetStageNimotu(GameMaxNimotu);
	NimotuCountUI->GameNimotuAnimation();
}

float AGameController::GetNowTimeAngle()
{
	return GameTimeUI->GetNowTimeAngle();
}

float AGameController::GetNormaTimeAngle()
{
	return GameTimeUI->GetNormaTimeAngle();
}

void AGameController::PauseCall()
{
	//�A�^�b�N�A�j�����̓|�[�Y���J���Ȃ��悤�ɂ���
	if (IsGameClear)return;
	if (IsGameOver)return;
	if (GetPlayer->GetIsAttack())return;
	if (GetPlayer->GetIsCharge())return;
	IsPause = true;
	if (!UGameplayStatics::IsGamePaused(GetWorld()))
	{
		if (PauseUIClass != nullptr)
		{//���߂ă|�[�Y��ʂ��J���Ƃ���UI�𐶐�����
			if (!PauseUI)
			{
				PauseUI = CreateWidget<UPauseUI>(GetWorld(), PauseUIClass);
				PauseUI->AddToViewport();
				PauseUI->SetNormaAngle(GetNormaTimeAngle());
				PauseUI->SetNeedleAndBG_Material(GetNowTimeAngle());
				PauseUI->SetTimeLimit(TimeLimit);
				PauseUI->SetNormaTime(TimeLimit - NormaTime);
				SetTimeCountPause();
			}
			else if (PauseUI)
			{
				if (PauseUI->GetIsPlayAnimation())return;
				PauseUI->AddToViewport();
				PauseUI->SetNormaAngle(GetNormaTimeAngle());
				PauseUI->SetNeedleAndBG_Material(GetNowTimeAngle());
				PauseUI->SetTimeLimit(TimeLimit);
				PauseUI->SetNormaTime(TimeLimit - NormaTime);
				SetTimeCountPause();
			}
			//�������Ă�nullptr��������G���[���\��
			if (PauseUI == nullptr)
			{
				UE_LOG(LogTemp, Error, TEXT("PauseUI : %s"), L"Widget cannot create");
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("PauseUI : %s"), L"PauseUIClass is nullptr");
		}
	}
	else if (UGameplayStatics::IsGamePaused(GetWorld()))
	{
		if (!PauseUI)return;
		if (PauseUI->GetIsPlayAnimation())return;
		PauseUI->EndAnimation();
		IsPause = false;
	}
}

void AGameController::InputRightCall()
{
	if (IsGameOver)
	{
		if (!GameOverUI)return;
		if (GameOverUI->GetIsPlayAnimation())return;
		GameOverUI->NextSelectState();
	}
	if (!UGameplayStatics::IsGamePaused(GetWorld()))return;
	if (IsPause)
	{
		if (!PauseUI)return;
		PauseUI->NextSelectState();
	}
	else if (IsResult)
	{
		if (!IsGameClear)return;
		if (!ResultUI)return;
		ResultUI->NextSelectState();
	}
}

void AGameController::InputLeftCall()
{
	if (IsGameOver)
	{
		if (!GameOverUI)return;
		if (GameOverUI->GetIsPlayAnimation())return;
		GameOverUI->BackSelectState();
	}
	if (!UGameplayStatics::IsGamePaused(GetWorld()))return;
	if (IsPause)
	{
		if (!PauseUI)return;
		PauseUI->BackSelectState();
	}
	else if (IsResult)
	{
		if (!IsGameClear)return;
		if (!ResultUI)return;
		ResultUI->BackSelectState();
	}
}

void AGameController::InputSelectCall()
{
	if (IsGameOver)
	{
		if (!GameOverUI)return;
		if (GameOverUI->GetIsPlayAnimation())return;
		GameOverUI->SelectStateAction();
	}
	if (!UGameplayStatics::IsGamePaused(GetWorld()))return;
	if (IsPause)
	{
		if (!PauseUI)return;
		PauseUI->SelectStateAction();
		IsPause = false;
	}
	else if (IsResult)
	{
		if (!IsGameClear)return;
		if (!ResultUI)return;
		ResultUI->SelectStateAction();
	}
}

void AGameController::RemoveUI()
{
	if (GameTimeUI)
	{
		GameTimeUI->RemoveFromParent();
	}
	if (NimotuCountUI)
	{
		NimotuCountUI->RemoveFromParent();
	}
	if (ControlTipsUI)
	{
		ControlTipsUI->RemoveFromParent();
	}
	GetPlayer->HammerCountBarParent();
}