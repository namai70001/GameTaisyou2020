// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputManager.generated.h"

//@brief �A�N�V����1�ɑ΂�����̓X�e�[�g�Ǘ��N���X
class InputActionState
{
public:
	bool IsPress = false;//�����ꂽ���ǂ���
	bool IsKeep = false;//�����ꑱ���Ă��邩�ǂ���
	bool IsRelease = false;//�����ꂽ���ǂ���

	//@brief �X�V����
	void Refresh() {
		IsPress = false;
		IsRelease = false;
	};

	//@brief �����ꂽ�ۂ̏���
	void Press() {
		if (IsKeep) return;
		IsKeep = true;
		IsPress = true;
		IsRelease = false;
	}

	//@berief �����ꂽ�ۂ̏���
	void Release() {
		if (!IsKeep) return;
		IsKeep = false;
		IsPress = false;
		IsRelease = true;
	}
};

//@brief ���͕����̍\����
struct InputDirection
{
	float Horizontal = 0.0f;
	float Vertical = 0.0f;
};

//@brief ���͏��\����
struct InputState
{
	// �ړ��Ȃǂ̓��͏��i���X�e�B�b�N�j
	InputDirection LeftStick;
	// �J��������Ȃǂ̓��͏��i�E�X�e�B�b�N�j
	InputDirection RightStick;
	// �v���C���[�A�^�b�N
	InputActionState Attack;
	//�|�[�Y�{�^��
	InputActionState Pause;
	//�X�e�[�W�I����ʂ�|�[�Y��ʂ̏���͏��
	InputActionState Up;
	//�X�e�[�W�I����ʂ�|�[�Y��ʂ̉����͏��
	InputActionState Down;
	//����{�^��
	InputActionState Select;
};

//@brief ���͊Ǘ��N���X
UCLASS()
class WAVE_API AInputManager : public APawn
{
	GENERATED_BODY()

	InputState State;
public:
	// Sets default values for this pawn's properties
	AInputManager();
	~AInputManager();

protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;

public:
	// �C���X�^���X���擾����
	static const AInputManager* GetInstance();

	const InputState * GetState() const { return &State; };

private:

	void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// ���X�e�B�b�N���͏��
	void InputLS_Horizontal(float Value) { State.LeftStick.Horizontal = Value; };	// ������
	void InputLS_Vertical(float Value) { State.LeftStick.Vertical = Value; };		// �c����

	// �E�X�e�B�b�N���͏��
	void InputRS_Horizontal(float Value) { State.RightStick.Horizontal = Value; };	// ������
	void InputRS_Vertical(float Value) { State.RightStick.Vertical = Value; };		// �c����

	// �n���}�[�ł��������͏��󂯎��
	void InputAttackPress() { State.Attack.Press(); };
	void InputAttackRelease() { State.Attack.Release(); };

	//�|�[�Y�{�^�����͏��󂯎��
	void InputPausePress() { State.Pause.Press(); };
	void InputPauseRelease() { State.Pause.Release(); };
	//�X�e�[�W�I����ʂ�|�[�Y��ʂ̏���͏��󂯎��
	void InputUpPress() { State.Up.Press(); };
	void InputUpRelease() { State.Up.Release(); };
	//�X�e�[�W�I����ʂ�|�[�Y��ʂ̉����͏��󂯎��
	void InputDownPress() { State.Down.Press(); };
	void InputDownRelease() { State.Down.Release(); };
	//����{�^�����͏��󂯎��
	void InputSelectPress() { State.Select.Press(); };
	void InputSelectRelease() { State.Select.Release(); };
};