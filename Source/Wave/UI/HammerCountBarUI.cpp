// Fill out your copyright notice in the Description page of Project Settings.


#include "HammerCountBarUI.h"
#define GAUGE_SPEED (0.03f)//�Q�[�W���X�Ɍ��炷�X�s�[�h
float UHammerCountBarUI::DownGauge(const float DamageX, const float HpX)
{
	if (!IsDamageDown)
	{//�Q�[�W�����߂ĉ����鎞�̍����𓾂�
		NowHp = DamageX - HpX;
		IsDamageDown = true;
	}
	//1�t���[���Ō��炷�ʂ��v�Z
	//���͊��������Œ�l�ɂ���Έ��̑����ɂȂ�
	float AddBar = NowHp * GAUGE_SPEED;
	return DamageX - AddBar;
}