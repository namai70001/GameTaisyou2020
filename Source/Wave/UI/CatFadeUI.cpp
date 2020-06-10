// Fill out your copyright notice in the Description page of Project Settings.


#include "CatFadeUI.h"

float UCatFadeUI::UpdateFadeTime(const float InDeltaTime)
{
	if (!IsEnable)return Scale;
	//�t�F�[�h���Ȃ�^�C�v�ɍ��킹�ăA���t�@�l��ς���
	if (IsFadeIn)
	{
		Scale += InDeltaTime * FadeSpeed;
		if (Scale >= 100.0f)
		{
			IsEnable = false;
		}
	}
	else
	{
		Scale -= InDeltaTime * FadeSpeed;
		if (Scale <= 0.0f)
		{
			IsEnable = false;
			RemoveFromParent();
		}
	}
	return Scale;
}