// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

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
};
