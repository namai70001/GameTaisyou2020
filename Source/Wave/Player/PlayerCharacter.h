// Fill out your copyright notice in the Description page of Project Settings.
// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Niagara/Classes/NiagaraSystem.h"	// �G�t�F�N�g�p
//generated.h�͈�ԍŌ�ɂ����Ȃ��Ƃ����Ȃ�
#include "PlayerCharacter.generated.h"
class UPauseUI;
class UPlayerAnimInstance;
class UHammerCountBarUI;
class AWaterSurface;
class ARaft;

UCLASS(config = Game)
class APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Camera)
		class AGameCameraActor* FollowCamera;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool IsDeth;	// ��O�ɏo���܂��͐��ɗ�������

private:
	// �|�[�YUI�@�G�f�B�^�Ŏw�肷��
	UPROPERTY(EditAnywhere)
		TSubclassOf<UPauseUI> PauseUIClass;
	UPROPERTY(EditAnywhere)
		TSubclassOf<UHammerCountBarUI> HammerCountBarUIClass;
	UPauseUI* PauseUI = nullptr;
	UHammerCountBarUI* HammerCountBarUI = nullptr;
	//�Q�[�W�ŕ\������p�̃v���C���[�̃n���}�[MAXHP
	UPROPERTY(EditAnywhere, Category = "Parameter")
		float MaxHammerHP = 100.0f;
	//1�t���[�����ɗ��߂�n���}�[�p���[
	UPROPERTY(EditAnywhere, Category = "Parameter")
		float ChargeSpeed = 0.7f;
	// ���ŗ��߂���ő�p���[
	UPROPERTY(EditAnywhere, Category = "Parameter")
		float ChargePowerMax = 100.0f;
	// CoolTime�񕜎��Ԕ{���i�����قǑ����j
	UPROPERTY(EditAnywhere, Category = "Parameter")
		float CoolTimeHealSpped = 0.8f;
	// HP�񕜎��ԁi�����قǑ����j
	UPROPERTY(EditAnywhere, Category = "Parameter")
		float HpHealSpped = 0.9f;
	// ���߂Ă���͂��J�E���g
	float ChargeCount= 0.0f;
	//�@���Ȃ�����
	float CoolTime = 0.0f;
	// �g���N������
	UPROPERTY(EditAnywhere, Category = "Parameter")
		float HammerPowerValue = 1.0f;
	// �v���C���[�̓����蔻��̔��a
	UPROPERTY(EditAnywhere, Category = "Parameter")
		float Radius = 100.0f;

	//UPROPERTY(EditAnywhere, Category = "Effect")
	//	bool  Now = 100.0f;

	float HammerHP;
	float MoveAmount;	// �ړ��ʂ̊���

	void PauseInput();
	void CreateHammerCountBarUI();
	void UpdateGaugeHP();
	AWaterSurface* Water;

	ARaft* CurrentRaft = nullptr;	// ����Ă��Ȃ����͏��nullptr
	bool IsInRaft = false;
	void ResetRaftParam();

public:
	APlayerCharacter();

	//virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		void BeginPlay_C();

	virtual void Tick(float DeltaTime) override;

	// �n���}�[�̐�[�̃g�����X�t�H�[��
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "C++Class")
		USceneComponent* HummerTipPoint = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C++Class")
		float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C++Class")
		float BaseLookUpRate;

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
		void ChageUpDateEmmiter(FVector pos);


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

protected:

	void Move(const FVector & Direction, float Value);

	bool CheckTraceGround(FHitResult & Result, const FVector & Start, float SphereRadius, AActor* Ignore = nullptr);

	//�L�[���������Ƃ��̃n���}�[�U��
	void TriggerHammerAttack(void);
	//�L�[�𗣂����Ƃ��̃n���}�[�U��
	void ReleaseHammerAttack(void);

	void HummerAttackEnd();
protected:

	//BlueprintReadWrite=Get��Set�֐����u���[�u�����g�Ō��J

	//�n���}�[�U���Ńz�[���h���Ȃ�true
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		bool IsAttackHold;
	//�U���A�j���Đ����Ȃ�true
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		bool IsPlayAttackAnime;
	//�n���}�[�p���[�ϐ�
	UPROPERTY(BlueprintReadWrite, Category = "C++Class")
		float HammerPower;

private:

	UPlayerAnimInstance* AnimInst;

	//���ʂɔg�����Ă�
	void WaterAttack(FVector Point, float Power);
	//�n���}�[����Q�[�W���}�C�i�X
	void MinusHammerGauge(const float Power);
	// �v���C���[���牺�Ƀ��C���΂��ė��̏ォ�ǂ������ׂ�
	bool CheckGround();
	// �J�����̌�������ړ��������Z�o
	FVector GetInputDirection(float VerticalValue, float HolizontalValue);
	// �ړ������Ƀv���C���[����������֐�
	void SetLookAt(FVector Direction, float Speed);
public:
	//HP�o�[�̃m���}�ʒu���Z�b�g
	void SetNormaPercent(const float percent);
	float GetMoveAmount() { return MoveAmount; };
	//�Q�[�WUI���\���ɂ���
	void HammerCountBarParent();
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
		FORCEINLINE	bool GetIsAttackHold() const { return IsAttackHold; };
};