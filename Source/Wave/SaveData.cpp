// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveData.h"

void USaveData::InSaveData(const int number, const FSaveDataStruct savedata)
{
	Save.EmplaceAt(number, savedata);
	//Save.Insert_GetRef(savedata,number);
}

FSaveDataStruct USaveData::GetSaveData(const int number) const
{
	//�z��̗v�f�O�A�N�Z�X���������̒l��Ԃ�
	if (number >= Save.Max())
	{
		return FSaveDataStruct();
	}
	return Save[number];
}
