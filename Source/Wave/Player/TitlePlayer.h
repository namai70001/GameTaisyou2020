// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TitlePlayer.generated.h"

UCLASS()
class WAVE_API ATitlePlayer : public ACharacter
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
		AActor* TargetPoint = nullptr;
	UPROPERTY(EditAnywhere)
		AActor* BattleTargetPoint = nullptr;
	UPROPERTY(EditAnywhere, Category = "Instanced")
		float MoveSpeed = 0.02f;//�f���Ɍ��������̈ړ����x
public:
	// Sets default values for this character's properties
	ATitlePlayer();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		bool IsMove = false;
	//�^�C�g������G���^�[�������ꂽ��true
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		bool IsSelect = false;
	//�^�[�Q�b�g�A�N�^�[�ɋ߂Â�����true
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		bool IsTarget = false;
public:	
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		void BeginPlay_C();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void TargetRotation(const bool type);//�^�[�Q�b�g�̕��Ɍ���
	void TargetMove(const bool type);//�^�[�Q�b�g�̕��Ɉړ�����
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		void SetIsMove(const bool type) { IsMove = type; };
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		void SetIsSelect(const bool type) { IsSelect = type; };
};
