// Fill out your copyright notice in the Description page of Project Settings.


#include "VersusController.h"
#include "Kismet/GameplayStatics.h"
#include "../Wave/Object/GoalComponent.h"
#include "Blueprint/UserWidget.h"
#include "../Wave/UI/ControlTipsUI.h"
#include "../Wave/UI/BattleTimeUI.h"
#include "../Wave/UI/BattleResultUI.h"
#include "../Wave/UI/BattleGameStartUI.h"
#include "../Wave/Player/PlayerCharacter.h"
#include "../Wave/WaterSurface/FloatActor.h"
#include "TimerManager.h"
#include "../Wave/SoundManager.h"
#include "../Wave/Camera/GameCameraFocusPoint.h"
#include "../Wave/UI/FadeUI.h"
#include "../Wave/Camera/GameCameraActor.h"
#include "Components/MaterialBillboardComponent.h"
#include "../Wave/SoundManager.h"
#include "../Wave/WaterSurface/WaterSurface.h"
#include "Engine/GameViewportClient.h"
#include "SectionController.h"
#include "WaterSurface/WaterSurface.h"
#include "UI/BattleResultUI.h"
#include "InputManager.h"
// Sets default values
AVersusController::AVersusController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AVersusController::BeginPlay()
{
	Super::BeginPlay();

	// ��ʕ����L��
	GetWorld()->GetGameViewport()->SetForceDisableSplitscreen(false);

	// �e�v���C���[���擾
	TArray<class AActor*> FoundPlayers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), FoundPlayers);
	for (auto Actor : FoundPlayers)
	{
		// �擾�����A�N�^�[���v���C���[���ǂ����𔻒�
		APlayerCharacter * Player = Cast<APlayerCharacter>(Actor);
		if (!Player) continue;

		// �v���C���[�ԍ����擾����1,2���ꂼ��ɕۑ�����
		if (Player->GetBattleNumber() == 1)
		{
			GetPlayer1 = Player;
			Player1RespawnLocation = Player->GetActorLocation();
		}
		else if (Player->GetBattleNumber() == 2)
		{
			GetPlayer2 = Player;
			Player2RespawnLocation = Player->GetActorLocation();
		}
	}

	IsStartResultEvent = false;

	//�|�[�Y���ł�Tick������悤�ɂ���
	SetTickableWhenPaused(true);

	// �f�o�b�O�X�N���[�����[�h�Ȃ�v���C���[��A�C�R�����\����
	if (DebugScreenMode)
	{
		GetPlayer1->SetPlayerHiddenInGame();
		GetPlayer1->DebugHammerCountBarParent();
		GetPlayer1->SetNoTick();
		GetPlayer2->SetPlayerHiddenInGame();
		GetPlayer2->DebugHammerCountBarParent();
		GetPlayer2->SetNoTick();
		SetAllInvisibleStageIcon();
		return;
	}

	// TODO::�o�g���p�̎��Ԃ�UI���쐬
	//CreateGameTimeUI();

	// TODO::�o�g���p��UI�ɍ쐬
	//GetPlayer1->CreateHammerCountBarUI();
	//GetPlayer2->CreateHammerCountBarUI();
	if (IsTips)
	{
		CreateControlTipsUI();
	}
	CreateBattleTimeUI();
	if (BattleTimeUI)
	{
		StartTimeCount();
	}
	//�Ō�Ƀt�F�[�h�A�E�g���o�����ߍŌ�ɐ���
	InitFadeOut();
	GetPlayer1->SetNoTickSingle(true);
	GetPlayer2->SetNoTickSingle(true);
	// BGM���Đ�
	AudioComponent = ASoundManager::CreateAudioComponent(SOUND_TYPE::STAGE_BGM);
	AudioComponent->Play(0.0f);
}

void AVersusController::CreateSections()
{
	for (int i = 0; i < SectionCount; i++)
	{
		int index = FMath::RandRange(0, Sections.Num() - 1);
		AddSection(index, 1, i);
		AddSection(index, 2, i);
	}
}

void AVersusController::AddSection(int index, int PlayerNumber, int Cnt)
{
	FVector Location = FVector::ZeroVector;
	Location.X = SectionSize * Cnt;
	Location.Y = 1500 * (PlayerNumber - 1);
	ASectionController* section = GetWorld()->SpawnActor<ASectionController>(Sections[index]);
	section->SetActorLocation(Location);
	(PlayerNumber == 1 ? Player1Sections : Player2Sections).Add(section);
}

// Called every frame
void AVersusController::Tick(float DeltaTime)
{
	if (DebugScreenMode)return;
	if (FadeUI->GetFadeIsEnable())return;
	if (BattleGameStartUIClass != nullptr)
	{
		if (!FadeUI->GetFadeIsEnable() && !BattleGameStartUI)
		{
			//�t�F�[�h�A�E�g�I������J�E���g�_�E�����J�n����悤�ɂ���
			CreateBattleGameStartUI();
		}
		if (BattleGameStartUI->GetIsCountDownZero() && GetPlayer1->GetNoTick())
		{
			GetPlayer1->SetNoTickSingle(false);
			GetPlayer2->SetNoTickSingle(false);
		}
	}
	Super::Tick(DeltaTime);

	CheckPlayerFall();

	UpdateTime();
	if (GetLimitTimeZero())
	{
		BattleFinish();
	}
	if (IsTimeOver)
	{
		CreateBattleResultUI();
		UGameplayStatics::SetGamePaused(GetWorld(), true);
	}
}

void AVersusController::InputBattleResultUI()
{
	if (!BattleResultUI)return;
	if (BattleResultUI->GetIsNoInput())return;
	const AInputManager * inputManager = AInputManager::GetInstance();
	if (!inputManager)return;

	const InputState * input = inputManager->GetState();
	if (input->Right.IsPress)
	{
		BattleResultUI->NextSelectState();
	}
	if (input->Left.IsPress)
	{
		BattleResultUI->BackSelectState();
	}
	if (input->Select.IsPress)
	{
		BattleResultUI->SelectStateAction();
	}
}

void AVersusController::InitFadeOut()
{
	if (FadeUI)return;
	if (FadeUIClass != nullptr)
	{
		FadeUI = CreateWidget<UFadeUI>(GetWorld(), FadeUIClass);
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

void AVersusController::CreateControlTipsUI()
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

void AVersusController::CreateBattleTimeUI()
{
	if (BattleTimeUI)return;
	if (BattleTimeUIClass != nullptr)
	{
		BattleTimeUI = CreateWidget<UBattleTimeUI>(GetWorld(), BattleTimeUIClass);
		if (BattleTimeUI != nullptr)
		{
			BattleTimeUI->AddToViewport();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("BattleTimeUIClass : %s"), L"Widget cannot create");
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BattleTimeUIClass : %s"), L"UIClass is nullptr");
	}
}

void AVersusController::CreateBattleResultUI()
{
	if (BattleResultUI)return;
	if (BattleResultUIClass != nullptr)
	{
		BattleResultUI = CreateWidget<UBattleResultUI>(GetWorld(), BattleResultUIClass);
		if (BattleResultUI != nullptr)
		{
			BattleResultUI->AddToViewport();
			if (Player1Score >= Player2Score)
			{
				BattleResultUI->SetWinPlayer(true);
			}
			else
			{
				BattleResultUI->SetWinPlayer(false);
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("BattleResultUIClass : %s"), L"Widget cannot create");
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BattleResultUIClass : %s"), L"UIClass is nullptr");
	}
}

void AVersusController::CreateBattleGameStartUI()
{
	if (BattleGameStartUI)return;
	if (BattleGameStartUIClass != nullptr)
	{
		BattleGameStartUI = CreateWidget<UBattleGameStartUI>(GetWorld(), BattleGameStartUIClass);
		if (BattleGameStartUI != nullptr)
		{
			BattleGameStartUI->AddToViewport();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("BattleGameStartUIClass : %s"), L"Widget cannot create");
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BattleGameStartUIClass : %s"), L"UIClass is nullptr");
	}
}

void AVersusController::UpdateTime()
{
	//�Q�[�����I�����Ă����玞�v��i�߂Ȃ�
	if (IsBatlleFinish)return;
	if (UGameplayStatics::IsGamePaused(GetWorld()))
	{
		SetTimeCountPause();
		return;
	}
	if (!UGameplayStatics::IsGamePaused(GetWorld()))
	{
		SetTimeCountRePlay();
	}

	// �o�g���p���Ԃ�UI�̍X�V���s��
	//if (GameTimeUI)
	//{
	//	GameTimeUI->UpDateTime();
	//}
}

bool AVersusController::GetLimitTimeZero()
{
	// TODO::�o�g���p�̎��Ԃ�UI���Q�Ƃ��Ď��ԏI�����ǂ����𔻒肷��
	//if (GameTimeUI)
	//{
	//	//�J�E���g�_�E���Ɛj��0���w���Ă����玞�ԏI���Ƃ���
	//	if (GameTimeUI->GetIsCountZero() && GameTimeUI->GetIsTimeEnd())
	//	{
	//		return true;
	//	}
	//}
	return false;
}

void AVersusController::SetTimeCountPause()
{
	// TODO::�o�g���p�̎��Ԃ�UI���ꎞ��~
	//if (GameTimeUI)
	//{
	//	GameTimeUI->AnimationPause();
	//}
}

void AVersusController::SetTimeCountRePlay()
{
	// TODO::�o�g���p�̎��Ԃ�UI���ĊJ
	//if (GameTimeUI)
	//{
	//	GameTimeUI->AnimationRePlay();
	//}
}

float AVersusController::GetNowTimeAngle()
{
	// �o�g���p���Ԃt�h���猻�݂̐j�̊p�x���擾����
	//return GameTimeUI->GetNowTimeAngle();
	return 0;
}

void AVersusController::PauseCall()
{
	//�A�^�b�N�A�j�����̓|�[�Y���J���Ȃ��悤�ɂ���
	if (IsBatlleFinish)return;
	if (GetPlayer1->GetIsAttack())return;
	if (GetPlayer1->GetIsCharge())return;
	if (GetPlayer2->GetIsAttack())return;
	if (GetPlayer2->GetIsCharge())return;
	IsPause = true;
	if (!UGameplayStatics::IsGamePaused(GetWorld()))
	{
		// TODO::�o�g���p�|�[�YUI���쐬���ĊJ��
	}
	else if (UGameplayStatics::IsGamePaused(GetWorld()))
	{
		if (!PauseUI)return;
		IsPause = false;
		// TODO::�o�g���p�|�[�YUI�����
	}
}

void AVersusController::InputRightCall()
{
	// TODO::�|�[�Y�Ƃ����U���g��UI���ł�����A�����̑�����L�q
}

void AVersusController::InputLeftCall()
{
	// TODO::�|�[�Y�Ƃ����U���g��UI���ł�����A�����̑�����L�q
}

void AVersusController::InputSelectCall()
{
	// TODO::�|�[�Y�Ƃ����U���g��UI���ł�����A�����̑�����L�q
}

void AVersusController::StopBGM()
{
	if (!AudioComponent) return;
	if (!(AudioComponent->IsPlaying())) AudioComponent->Stop();
}

void AVersusController::RemoveUI()
{
	// TODO::�o�g���p����UI���\���ɂ���

	if (ControlTipsUI)
	{
		ControlTipsUI->RemoveFromParent();
	}
	GetPlayer1->HammerCountBarParent();
	GetPlayer2->HammerCountBarParent();
}

void AVersusController::SetAllInvisibleStageIcon()
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), Actors);

	for (auto Actor : Actors)
	{
		TArray<UActorComponent*> Billboards;
		Actor->GetComponents(UMaterialBillboardComponent::StaticClass(), Billboards);

		for (auto Billboard : Billboards)
		{
			Cast<UMaterialBillboardComponent>(Billboard)->SetRenderInMainPass(false);
		}
	}
}

void AVersusController::BattleFinish()
{
	// TODO::���s�̔�������ăo�g���p���U���gUI���J��

	IsBatlleFinish = true;
}

void AVersusController::CheckPlayerFall()
{
	if (!GetPlayer1)return;
	if (GetPlayer1->GetIsDeth())
	{
		// �������ƃv���C���[�����X�|�[�������đ���̃X�R�A�����Z
		GetPlayer1->PlayerRespawn(Player1RespawnLocation);
	}

	if (!GetPlayer2)return;
	if (GetPlayer2->GetIsDeth())
	{
		GetPlayer2->PlayerRespawn(Player2RespawnLocation);
	}
}