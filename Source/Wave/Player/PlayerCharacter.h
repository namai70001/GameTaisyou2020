// Fill out your copyright notice in the Description page of Project Settings.
// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Niagara/Classes/NiagaraSystem.h"	// �G�t�F�N�g�p
#include "../InputManager.h"
//generated.h�͈�ԍŌ�ɂ����Ȃ��Ƃ����Ȃ�
#include "PlayerCharacter.generated.h"
class UPauseUI;
class UPlayerAnimInstance;
class UHammerCountBarUI;
class AWaterSurface;
class ARaft;
class AGameCameraActor;

UCLASS(config = Game)
class APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Camera)
		AGameCameraActor* FollowCamera;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool IsDeth;	// ��O�ɏo���܂��͐��ɗ�������

private:
	UPROPERTY(EditAnywhere)
		TSubclassOf<UHammerCountBarUI> HammerCountBarUIClass;
	UHammerCountBarUI* HammerCountBarUI = nullptr;
	//�i�K���Ƃ̃n���}�[�p���[���i�[
	UPROPERTY(EditAnywhere)
		TArray<float> HammerPowerArray;
	//�Q�[�W�ŕ\������p�̃v���C���[�̃n���}�[MAXHP
	UPROPERTY(EditAnywhere, Category = "Parameter")
		float MaxHammerHP = 100.0f;
	//1�t���[�����ɗ��߂�n���}�[�p���[
	UPROPERTY(EditAnywhere, Category = "Parameter")
		float ChargeSpeed = 1.0f;
	// ���ŗ��߂���ő�p���[
	UPROPERTY(EditAnywhere, Category = "Parameter")
		float ChargePowerMax = 100.0f;
	// CoolTime�񕜎��Ԕ{���i�����قǑ����j
	UPROPERTY(EditAnywhere, Category = "Parameter")
		float CoolTimeHealSpped = 14.0f;
	// HP�񕜎��ԁi�����قǑ����j
	UPROPERTY(EditAnywhere, Category = "Parameter")
		float HpHealSpped = 0.8f;
	// ���߂Ă���͂��J�E���g
	float ChargeCount= 0.0f;
	// �g���N������
	UPROPERTY(EditAnywhere, Category = "Parameter")
		float HammerPowerValue = 1.0f;
	// �o�g�����[�h�ł̔ԍ�(0�Ȃ�ʏ탂�[�h)
	UPROPERTY(EditAnywhere)
		int BattleNumber = 0;
	// �v���C���[�̓����蔻��̔��a
	//UPROPERTY(EditAnywhere, Category = "Parameter")
		float CollisionRadius = 0.0f;

	//UPROPERTY(EditAnywhere, Category = "Effect")
	//	bool  Now = 100.0f;

	float HammerHP;
	float MoveAmount;	// �ړ��ʂ̊���
	
	void UpdateGaugeHP();
	AWaterSurface* Water;

	ARaft* CurrentRaft = nullptr;	// ����Ă��Ȃ����͏��nullptr
	bool IsInRaft = false;
	void ResetRaftParam();

	EAutoReceiveInput::Type InputManagerIndex = EAutoReceiveInput::Player0;

	UAudioComponent* AudioComponent;
public:
	APlayerCharacter();

	//virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		void BeginPlay_C();

	virtual void Tick(float DeltaTime) override;

	// �n���}�[�̐�[�̃g�����X�t�H�[��
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "C++Class")
		USceneComponent* HummerTipPoint = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
		UNiagaraSystem* AttackEffect = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
		float AttackEffectScale = 1.0f;

	UFUNCTION(BlueprintCallable, Category = "C++Library", BlueprintImplementableEvent)
		void OutLineDraw();
	UFUNCTION(BlueprintCallable, Category = "C++Library", BlueprintImplementableEvent)
		void OutLineNotDraw();
	UFUNCTION(BlueprintCallable, Category = "C++Library", BlueprintImplementableEvent)
		void PlayerDeth();

	UFUNCTION(BlueprintCallable, Category = "C++Library", BlueprintImplementableEvent)
		void ChageCreateEmmiter();
	UFUNCTION(BlueprintCallable, Category = "C++Library", BlueprintImplementableEvent)
		void ChageDestroyEmmiter();
	UFUNCTION(BlueprintCallable, Category = "C++Library", BlueprintImplementableEvent)
		void ChageUpDateEmmiter();
	UFUNCTION(BlueprintCallable, Category = "C++Library", BlueprintImplementableEvent)
		void ImpactEmmiterCreate(const FVector & Pos);

	static FORCEINLINE bool Trace(
		UWorld* World,
		AActor* ActorToIgnore,
		const FVector& Start,
		const FVector& End,
		FHitResult& HitOut,
		ECollisionChannel CollisionChannel = ECC_Pawn,
		bool ReturnPhysMat = false)
	{
		if (!World)
		{
			return false;
		}

		FCollisionQueryParams TraceParms(FName(TEXT("VictoreCore Trace")), true, ActorToIgnore);
		TraceParms.bTraceComplex = true;
		TraceParms.bReturnPhysicalMaterial = ReturnPhysMat;
		TraceParms.AddIgnoredActor(ActorToIgnore);
		HitOut = FHitResult(ForceInit);
		World->LineTraceSingleByChannel(
			HitOut,
			Start,
			End,
			CollisionChannel,
			TraceParms
			);


		return (HitOut.GetActor() != NULL);
	}

	UFUNCTION(BlueprintCallable, Category = "C++Library")
		void PlayerRespawn(const FVector & location);

protected:

	void Move(const FVector & Direction, float Value);

	bool CheckTraceGround(FHitResult & Result, const FVector & Start, float SphereRadius, AActor* Ignore = nullptr);

	//�L�[���������Ƃ��̃n���}�[�U��
	void TriggerHammerAttack(void);
	//�L�[�𗣂����Ƃ��̃n���}�[�U��
	void ReleaseHammerAttack(void);

	void HummerAttackEnd();
protected:

	//�n���}�[�p���[�ϐ�
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		float HammerPower;

private:
	bool NoTick = false;
	UPlayerAnimInstance* AnimInst;

	//���ʂɔg�����Ă�
	void WaterAttack(FVector Point, float Power, float Radius);
	//�n���}�[����Q�[�W���}�C�i�X
	void MinusHammerGauge(const float Power);
	// �v���C���[���牺�Ƀ��C���΂��ė��̏ォ�ǂ������ׂ�
	bool CheckGround();
	// �J�����̌�������ړ��������Z�o
	FVector GetInputDirection(float VerticalValue, float HolizontalValue);
	// �ړ������Ƀv���C���[����������֐�
	void SetLookAt(FVector Direction, float Speed);
	//�n���}�[�p���[�z�񂩂�p���[���󂯎��
	float GetArrayHammerPower();
public:
	float GetMoveAmount() { return MoveAmount; };
	void CreateHammerCountBarUI();
	//�Q�[�WUI���\���ɂ���
	void HammerCountBarParent();
	//�Q�[�WUI���\���ɂ���(�f�o�b�O�p�j
	void DebugHammerCountBarParent();
	//Tick��OFF�ɂ���
	void SetNoTick();
	//NoTick�݂̂̃t���O�������ŗ^�����l�ɂ���
	void SetNoTickSingle(const bool type) { NoTick = type; };
	void SetGameFinish(bool isClear = true);
	FORCEINLINE	bool GetNoTick() const { return NoTick; }
	//�v���C���[���\���ɂ���
	UFUNCTION(BlueprintCallable, Category = "C++Library")
	void SetPlayerHiddenInGame();
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE	float GetMaxHammerHP() const { return MaxHammerHP; };
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE	float GetHammerHP() const { return HammerHP; };
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE	float GetHammerPower() const { return HammerPower; };
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE	float GetChargeCount() const { return ChargeCount; };
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE	bool GetIsDeth() const { return IsDeth; };
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		FORCEINLINE	float GetBattleNumber() const { return BattleNumber; };
	bool GetIsAttack() const;
	bool GetIsCharge() const;

	AGameCameraActor* GetCameraActor();
};