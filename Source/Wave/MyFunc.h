// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

// 2D�p�̃��C
struct FRay2D
{
	FRay2D() {};
	FRay2D(FVector2D Origin, FVector2D Direction) : Origin(Origin), Direction(Direction) {};
	FRay2D(FVector Origin, FVector Direction) : Origin(Origin), Direction(Direction) {};
	FVector2D Origin;
	FVector2D Direction;
};

// ����Ȃ��֐���⑫����N���X
//----------------------------------------------------
// ���ʂ���֐��Œǉ����������̂�����΂����R�ɂǂ���
//----------------------------------------------------
class UTexture2D;
class WAVE_API MyFunc
{
public:
	// �N�I�[�^�j�I����Axis��Angle���琶������
	static FQuat FromAxisAngleToQuaternion(const FVector & axis, float angle);
	//�t�@�C���p�X�Ŏw�肵���e�N�X�`������Ԃ�
	static UTexture2D* LoadTexture2DFromAssetPath(const FName AssetPath);
	//�Q�[����Ƀf�o�b�O�����\��
	//����1 �����F FColor::Green���Ŏw��\
	//����2 ���b�Z�[�W
	static void DisplayDebugMessage(const FColor DisplayColor,FString message);

	// �����̏Փ�
	static bool ColSegments(
		const FRay2D &seg1,          // ����1
		const FRay2D &seg2,          // ����2
		FVector2D* outPos = 0 // ��_�i�o�́j
	);
};
