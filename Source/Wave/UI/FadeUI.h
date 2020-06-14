// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Math/Color.h"
#include "FadeUI.generated.h"

/**
 * 
 */
UCLASS()
class WAVE_API UFadeUI : public UUserWidget
{
	GENERATED_BODY()
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
private:
	FLinearColor FadeColor;
	bool IsFadeIn;
	bool IsEnable;//Fade���Ȃ�true
	bool IsSceneFade;//�t�F�[�h�I�����ɃV�[���J�ڂ���Ȃ�true
	float FadeSpeed;	
	FName NextLevelName;// ���̃��x���̖��O
public:
	UFadeUI(const FObjectInitializer& ObjectInitializer);
	UFUNCTION(BlueprintCallable, Category = "C++Library")
	FORCEINLINE FLinearColor GetFadeColor() const { return FadeColor; }
	UFUNCTION(BlueprintCallable, Category = "C++Library")
	FORCEINLINE bool GetFadeIsEnable() const { return IsEnable; }
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void CatFadeEvent(const bool fade_in, const float fade_speed);
	//�t�F�[�h��Ԃɂ���
	//������ �t�F�[�h�F
	//������ true�Ńt�F�[�h�C���Afalse�Ńt�F�[�h�A�E�g
	//��O���� �t�F�[�h���x
	//��l���� �L�̃V���G�b�g�t�F�[�h���o���Ȃ�true
	//FLinearColor��FLinearColor::Black�̂悤�ɌĂяo�����Ƃ��ł���
	UFUNCTION(BlueprintCallable, Category = "C++Library")
	void SetFade(const FLinearColor fade_color,const bool fade_in,const float fade_speed,const bool IsCatFade)
	{
		FadeColor = fade_color;
		IsFadeIn = fade_in;
		IsEnable = true;
		IsSceneFade = false;
		FadeSpeed = 1.0f / fade_speed;
		if (IsFadeIn)
		{
			FadeColor.A = 0.0f;
		}
		else
		{
			FadeColor.A = 1.0f;
		}
		if (IsCatFade)
		{
			CatFadeEvent(IsFadeIn, fade_speed);
		}
	}
	//�t�F�[�h�C���ɂ���
	//������ �t�F�[�h�F
	//������ �t�F�[�h���x
	//��O���� �V�[���̖��O
	//��l���� �L�̃V���G�b�g�t�F�[�h���o���Ȃ�true
	UFUNCTION(BlueprintCallable, Category = "C++Library")
	void SetFadeLevel(const FLinearColor fade_color, const float fade_speed, const FName levelname, const bool IsCatFade)
	{
		FadeColor = fade_color;
		IsFadeIn = true;
		IsEnable = true;
		IsSceneFade = true;
		FadeSpeed = 1.0f / fade_speed;
		NextLevelName = levelname;
		if (IsFadeIn)
		{
			FadeColor.A = 0.0f;
		}
		else
		{
			FadeColor.A = 1.0f;
		}
		if (IsCatFade)
		{
			CatFadeEvent(IsFadeIn, fade_speed);
		}
	}
};
