// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameController.generated.h"

class UHammerCountUI;

// �Q�[�����̃N���A�����Ȃǂ��Ǘ�����N���X

UCLASS()
class WAVE_API AGameController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// �N���A���ǂ����̃S�[���̃m���}�J�E���g
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
		int NormaGoalCount = 0;
	//�n���}�[��@�����
	UPROPERTY(EditAnywhere, Category = "Game")
		int HammerCount = 0;

private:
	// �Q�[���N���A���ǂ���
	bool IsGameClear;
	// �V�[����̑S�ẴS�[���̃|�C���^����������
	TArray<class AGoal*> GoalArray;

	// �\������UI�@�G�f�B�^�Ŏw�肷��
	UPROPERTY(EditAnywhere)
		TSubclassOf<UHammerCountUI> UIClass;
	// �n���}�[�̃J�E���g��\������UI
	UHammerCountUI* HammerCountUI;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// �Q�[���N���A���ǂ����𔻒f����֐�
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE bool GetIsClear() const { return IsGameClear; }

	void MinusHammerCount();

private:
	// �S�[���ς݂̌����擾
	int GetGoalCount();
	//�n���}�[��@����񐔂�1�ȏ�Ȃ�UI�𐶐�
	void CreateHammerCountUI();
};
