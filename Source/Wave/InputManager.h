// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputManager.generated.h"

//@brief アクション1つに対する入力ステート管理クラス
class InputActionState
{
public:
	bool IsPress = false;//押されたかどうか
	bool IsKeep = false;//押され続けているかどうか
	bool IsRelease = false;//離されたかどうか

	//@brief 更新処理
	void Refresh() {
		IsPress = false;
		IsRelease = false;
	};

	//@brief 押された際の処理
	void Press() {
		if (IsKeep) return;
		IsKeep = true;
		IsPress = true;
		IsRelease = false;
	}

	//@berief 離された際の処理
	void Release() {
		if (!IsKeep) return;
		IsKeep = false;
		IsPress = false;
		IsRelease = true;
	}
};

//@brief 入力方向の構造体
struct InputDirection
{
	float Horizontal = 0.0f;
	float Vertical = 0.0f;
};

//@brief 入力情報構造体
struct InputState
{
	// 移動などの入力情報（左スティック）
	InputDirection LeftStick;
	// カメラ操作などの入力情報（右スティック）
	InputDirection RightStick;
	// プレイヤーアタック
	InputActionState Attack;
	// プレイヤーアタックキャンセル
	InputActionState AttackCancel;
	// カメラの追従タイプ切り替え（右スティック押し込みボタン）
	InputActionState RightStickButton;
	// ポーズボタン
	InputActionState Pause;
	// ステージ選択画面やポーズ画面の上入力情報
	InputActionState Up;
	// ステージ選択画面やポーズ画面の下入力情報
	InputActionState Down;
	// ステージ選択画面やポーズ画面の右入力情報
	InputActionState Right;
	// ステージ選択画面やポーズ画面の左入力情報
	InputActionState Left;
	// 決定ボタン
	InputActionState Select;
	// 戻るボタン
	InputActionState Back;
};

//@brief 入力管理クラス
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
	// インスタンスを取得する
	// Param : Index -> プレイヤー番号
	static const AInputManager* GetInstance(EAutoReceiveInput::Type Index = EAutoReceiveInput::Player0);

	const InputState * GetState() const { return &State; };

private:

	void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 左スティック入力情報
	void InputLS_Horizontal(float Value) { State.LeftStick.Horizontal = Value; };	// 横方向
	void InputLS_Vertical(float Value) { State.LeftStick.Vertical = Value; };		// 縦方向

	// 右スティック入力情報
	void InputRS_Horizontal(float Value) { State.RightStick.Horizontal = Value; };	// 横方向
	void InputRS_Vertical(float Value) { State.RightStick.Vertical = Value; };		// 縦方向

	// ハンマーでたたく入力情報受け取り
	void InputAttackPress() { State.Attack.Press(); };
	void InputAttackRelease() { State.Attack.Release(); };
	void InputAttackCancelPress() { State.AttackCancel.Press(); };
	void InputAttackCancelRelease() { State.AttackCancel.Release(); };
	
	// カメラの追従タイプ切り替え入力情報受け取り（右スティック押し込みボタン）
	void InputRightStickButtonPress() { State.RightStickButton.Press(); };
	void InputRightStickButtonRelease() { State.RightStickButton.Release(); };

	//ポーズボタン入力情報受け取り
	void InputPausePress() { State.Pause.Press(); };
	void InputPauseRelease() { State.Pause.Release(); };
	//ステージ選択画面やポーズ画面の上入力情報受け取り
	void InputUpPress() { State.Up.Press(); };
	void InputUpRelease() { State.Up.Release(); };
	//ステージ選択画面やポーズ画面の下入力情報受け取り
	void InputDownPress() { State.Down.Press(); };
	void InputDownRelease() { State.Down.Release(); };
	//ステージ選択画面やポーズ画面の右入力情報受け取り
	void InputRightPress() { State.Right.Press(); };
	void InputRightRelease() { State.Right.Release(); };
	//ステージ選択画面やポーズ画面の左入力情報受け取り
	void InputLeftPress() { State.Left.Press(); };
	void InputLeftRelease() { State.Left.Release(); };
	//決定ボタン入力情報受け取り
	void InputSelectPress() { State.Select.Press(); };
	void InputSelectRelease() { State.Select.Release(); };
	//戻るボタン入力情報受け取り
	void InputBackPress() { State.Back.Press(); };
	void InputBackRelease() { State.Back.Release(); };
};