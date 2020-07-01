// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleController.h"
#include "Kismet/GameplayStatics.h"
#include "GoalComponent.h"
#include "Blueprint/UserWidget.h"
#include "../UI/ControlTipsUI.h"
#include "../InputManager.h"
#include "../Player/PlayerCharacter.h"
#include "../WaterSurface/FloatActor.h"
#include "TimerManager.h"
#include "../SoundManager.h"
#include "../Camera/GameCameraFocusPoint.h"
#include "../UI/FadeUI.h"
#include "../Camera/GameCameraActor.h"
#include "Components/MaterialBillboardComponent.h"
#include "../SoundManager.h"
#include "../WaterSurface/WaterSurface.h"

// Sets default values
ABattleController::ABattleController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABattleController::BeginPlay()
{
	Super::BeginPlay();

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

	//�Ō�Ƀt�F�[�h�A�E�g���o�����ߍŌ�ɐ���
	InitFadeOut();

	// BGM���Đ�
	AudioComponent = ASoundManager::CreateAudioComponent(SOUND_TYPE::STAGE_BGM);
	AudioComponent->Play(0.0f);
}

// Called every frame
void ABattleController::Tick(float DeltaTime)
{
	if (DebugScreenMode)return;
	Super::Tick(DeltaTime);

	CheckPlayerFall();
	
	UpdateTime();
	if (GetLimitTimeZero())
	{
		BattleFinish();
	}
}

void ABattleController::InitFadeOut()
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

void ABattleController::CreateControlTipsUI()
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

void ABattleController::UpdateTime()
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

bool ABattleController::GetLimitTimeZero()
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

void ABattleController::SetTimeCountPause()
{
	// TODO::�o�g���p�̎��Ԃ�UI���ꎞ��~
	//if (GameTimeUI)
	//{
	//	GameTimeUI->AnimationPause();
	//}
}

void ABattleController::SetTimeCountRePlay()
{
	// TODO::�o�g���p�̎��Ԃ�UI���ĊJ
	//if (GameTimeUI)
	//{
	//	GameTimeUI->AnimationRePlay();
	//}
}

float ABattleController::GetNowTimeAngle()
{
	// �o�g���p���Ԃt�h���猻�݂̐j�̊p�x���擾����
	//return GameTimeUI->GetNowTimeAngle();
	return 0;
}

void ABattleController::PauseCall()
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

void ABattleController::InputRightCall()
{
	// TODO::�|�[�Y�Ƃ����U���g��UI���ł�����A�����̑�����L�q
}

void ABattleController::InputLeftCall()
{
	// TODO::�|�[�Y�Ƃ����U���g��UI���ł�����A�����̑�����L�q
}

void ABattleController::InputSelectCall()
{
	// TODO::�|�[�Y�Ƃ����U���g��UI���ł�����A�����̑�����L�q
}

void ABattleController::StopBGM()
{
	if (!AudioComponent) return;
	if (!(AudioComponent->IsPlaying())) AudioComponent->Stop();
}

void ABattleController::RemoveUI()
{
	// TODO::�o�g���p����UI���\���ɂ���

	if (ControlTipsUI)
	{
		ControlTipsUI->RemoveFromParent();
	}
	GetPlayer1->HammerCountBarParent();
	GetPlayer2->HammerCountBarParent();
}

void ABattleController::SetAllInvisibleStageIcon()
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

void ABattleController::BattleFinish()
{
	// TODO::���s�̔�������ăo�g���p���U���gUI���J��

	IsBatlleFinish = true;
}

void ABattleController::CheckPlayerFall()
{
	if (!GetPlayer1)return;
	if (GetPlayer1->GetIsDeth())
	{
		// �������ƃv���C���[�����X�|�[�������đ���̃X�R�A�����Z
		GetPlayer1->PlayerRespawn(Player1RespawnLocation);
		AActor* a = GetWorld()->SpawnActor<AActor>(RaftActor); 
		FVector RaftLocation = Player1RespawnLocation;
		RaftLocation.Z = 0;
		a->SetActorLocation(RaftLocation);
		Player2Score += 1;
	}

	if (!GetPlayer2)return;
	if (GetPlayer2->GetIsDeth())
	{
		GetPlayer2->PlayerRespawn(Player2RespawnLocation);
		AActor* a = GetWorld()->SpawnActor<AActor>(RaftActor);
		FVector RaftLocation = Player2RespawnLocation;
		RaftLocation.Z = 0;
		a->SetActorLocation(RaftLocation);
		Player1Score += 1;
	}
}