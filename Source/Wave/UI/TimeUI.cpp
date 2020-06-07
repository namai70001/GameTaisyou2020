// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeUI.h"

void UTimeUI::SetOneFrameAngle(const int time_limit)
{
	LimitTime = time_limit;
	//1�b�ŉ�]����l�����߂�
	float one = 360.0f / time_limit;
	//1�t���[���ŉ�]����l�����߂�
	OneFrameAngle = one / 60.0f;
}

void UTimeUI::SetCountDownAngle(const int countdown)
{
	//�܂�1�b���̉�]�l�����߂�
	CountDownAngle = OneFrameAngle * 60.0f;
	//1�b�̉�]�l�ƃJ�E���g�_�E�����Ԃ������ĉ�]�l�����߂�
	CountDownAngle = CountDownAngle * countdown;
	//360�������ĕ\�������]�l�����߂�
	CountDownAngle = 360.0f- CountDownAngle;
}

float UTimeUI::SetNormaAngle(const int norma_time)
{
	//�܂�1�b���̉�]�l�����߂�
	NormaAngle = OneFrameAngle * 60.0f;
	//1�b�̉�]�l�ƃJ�E���g�_�E�����Ԃ������ĉ�]�l�����߂�
	NormaAngle = NormaAngle * norma_time;
	//360�������ĕ\�������]�l�����߂�
	NormaAngle = 360.0f - NormaAngle;
	return NormaAngle;
}
