// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Spawner.generated.h"

UENUM(BlueprintType)
enum class ACTOR_TYPE : uint8
{
	FLOAT_ACTOR	UMETA(DisplayName = "FlaotActor"),
	BOMB		UMETA(DisplayName = "Bomb"),
};

UCLASS()
class WAVE_API ASpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawner();

	UFUNCTION(BlueprintCallable, Category = "C++Library")
	void Spawn();

	UFUNCTION(BlueprintCallable, Category = "C++Library")
	void SpawnCountStart();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere)
		ACTOR_TYPE type;	// �X�|�[������BP�̃^�C�v

	UPROPERTY(EditAnywhere)
		float interval = 1.0f;	// �X�|�[�����o

	UPROPERTY(EditAnywhere)
		bool isAutoSpawn = true;	 // true�Ȃ玩���ŃX�|�[��

	UPROPERTY(EditAnywhere)
		bool isLoop = true;		// �X�|�[�����J��Ԃ����H(false�Ȃ��񂾂�

	bool isEnd = false;			// true�Ȃ�����X�|�[�����Ȃ�
	bool isCounting = false;	// true�Ȃ�J�E���g����

	float currentTime = 0;

	TSubclassOf<class AActor> sc;	// �X�|�[������N���X���i�[
};
