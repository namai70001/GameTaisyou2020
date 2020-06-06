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

struct FRay2DCastInfo
{
	FVector2D NearPos;
	FVector2D NearNormal;
	FRay2D NearSideRay;
	bool IsHit = false;
	float HitDist = 0.0f;
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
	static bool ColSegments(const FRay2D &seg1, const FRay2D &seg2,	FVector2D* outPos = nullptr);
	// �����Ɖ~�̏Փ�
	static bool Check_Ray2D_VS_Circle(FRay2DCastInfo & Info, const FRay2D &Ray, const FVector2D & CirclePos, float CircleRadius);
	// ��̃x�N�g���̔��˃x�N�g�����擾
	static FVector2D GetReflectVector2D(const FVector2D & A, const FVector2D & B)
	{
		return (A - 2.0f * FVector2D::DotProduct(A, B) * B);
	};

	// ���C�̌�_�𒲂ׂċ������擾����֐�
	static bool Check_Ray2D_VS_Ray2D(FRay2DCastInfo & Info, FRay2D RayA, const FRay2D & RayB, const FVector2D & Normal);
	// �K���ɍ��������o�O����
	static bool Check_CircleRay2D_VS_Ray2D(FRay2DCastInfo & Info, FRay2D RayA, float CircleRadius, const FRay2D & RayB, const FVector2D & Normal);
};
