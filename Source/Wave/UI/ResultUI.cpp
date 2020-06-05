// Fill out your copyright notice in the Description page of Project Settings.


#include "ResultUI.h"
#include "../InputManager.h"

void UResultUI::NativeConstruct()
{
	IsPlayAnimation = false;
	ClearPlayAnimation();
	State = ResultState::RESULT_ANIME;
}

void UResultUI::NextSelectState()
{
	if (State != ResultState::INPUT)return;
	if (!IsDataTable)return;
	SelectNumber++;
	if (SelectNumber > static_cast<int>(ResultSelectState::STAGESELECT))
	{
		SelectNumber = static_cast<int>(ResultSelectState::NEXTSTAGE);
	}
}

void UResultUI::BackSelectState()
{
	if (State != ResultState::INPUT)return;
	if (!IsDataTable)return;
	SelectNumber--;
	if (SelectNumber < static_cast<int>(ResultSelectState::NEXTSTAGE))
	{
		SelectNumber = static_cast<int>(ResultSelectState::STAGESELECT);
	}
}

void UResultUI::SelectStateAction()
{
	if (State == ResultState::ENTER_WAIT)
	{
		State = ResultState::INPUT;
		return;
	}
	if (State != ResultState::INPUT)return;
	//���̃X�e�[�W���Ȃ������玟�̃X�e�[�W�ւ��o���Ȃ�����
	switch (SelectNumber)
	{
		case static_cast<int>(ResultSelectState::NEXTSTAGE) :
			NextStageChenge();
			break;
			case static_cast<int>(ResultSelectState::STAGESELECT) :
				StageSelectChenge();
				break;
	}
	State = ResultState::END;
}

void  UResultUI::SetStageSelectState()
{
	SelectNumber = static_cast<int>(ResultSelectState::STAGESELECT);
}