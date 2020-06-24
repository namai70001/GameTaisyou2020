// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawner.h"

static FString ActorPaths[] =
{
	TEXT("/Game/Main/MapObject/FloatActor1_Blueprint.FloatActor1_Blueprint_C"),
	TEXT("/Game/Main/MapObject/BomBP.BomBP_C"),
};

// Sets default values
ASpawner::ASpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASpawner::BeginPlay()
{
	Super::BeginPlay();

	sc = TSoftClassPtr<AActor>(FSoftObjectPath(*ActorPaths[(int)type])).LoadSynchronous(); // �p�X�ɊY������N���X���擾

	isEnd = false;

	// �����X�|�[���ł���΃X�|�[���J�E���g�J�n
	if (isAutoSpawn) 
		isCounting = true;
	else				
		isCounting = false;
}

// Called every frame
void ASpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// �J�E���g���Ȃ��Ȃ�I���
	if (!isCounting) return;

	// �J�E���g
	currentTime += 1.0f / 60.0f;

	// �ڕW���ԂɒB���ĂȂ���ΏI��
	if (currentTime < interval) return;

	Spawn();

	if (isAutoSpawn)
	{
		currentTime -= interval;
	}
	else
	{
		isCounting = false;
		currentTime = 0;
	}
}

void ASpawner::Spawn()
{
	// �I����Ă�Ή������Ȃ�
	if (isEnd) return;

	if (sc != nullptr)
	{
		AActor* a = GetWorld()->SpawnActor<AActor>(sc); // �X�|�[������
		a->SetActorLocation(GetActorLocation()); //�@���W��ݒ�
	}

	//�@���[�v���Ȃ��̂ł���΂����X�|�[�����Ȃ��悤�ɂ���
	if(!isLoop)isEnd = true;
}

void ASpawner::SpawnCountStart()
{
	isCounting = true;
};
