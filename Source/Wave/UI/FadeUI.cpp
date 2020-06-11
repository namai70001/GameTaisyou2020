// Fill out your copyright notice in the Description page of Project Settings.

#include "FadeUI.h"
#include "Kismet/GameplayStatics.h"

UFadeUI::UFadeUI(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), IsEnable(false)
{

}

void UFadeUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	if (!IsEnable)return;
	//�t�F�[�h���Ȃ�^�C�v�ɍ��킹�ăA���t�@�l��ς���
	Super::NativeTick(MyGeometry,InDeltaTime);
	if (IsFadeIn)
	{
		FadeColor.A += InDeltaTime * FadeSpeed;
		if (FadeColor.A >= 1.0f)
		{
			IsEnable = false;
			if (IsSceneFade)
			{
				UGameplayStatics::OpenLevel(this, NextLevelName, true);
			}
		}
	}
	else
	{
		FadeColor.A -= InDeltaTime * FadeSpeed;
		if (FadeColor.A <= 0.0f)
		{
			IsEnable = false;
		}
	}
	
}