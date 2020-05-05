#include "SoundManager.h"

//�C���X�^���X�ւ̃A�h���X
static ASoundManager* SoundManagerInstance = nullptr;

// Sets default values
ASoundManager::ASoundManager()
{
	PrimaryActorTick.bCanEverTick = true;

	SoundManagerInstance = this;
}

void ASoundManager::BeginPlay() {
	if (AudioComponent.Num() == SoundList.Num())
	{
		return;
	}

	for (const auto& Cue : SoundList)
	{
		FString CueName = Cue->GetName();
		FName AudioCompName(*CueName);

		// create AudioComponent
		auto NewAudioComponent = NewObject<UAudioComponent>(this, AudioCompName);

		AudioComponent.Add(NewAudioComponent);
	}
}

void ASoundManager::PlaySound(int Index)
{
	if (Index < 0 || Index >= SoundList.Num()) return;//���X�g�ɑ��݂��Ȃ��C���f�b�N�X�Ȃ�Đ����Ȃ�
	if (SoundList.GetData()[Index] == nullptr) return;//�T�E���h���ݒ肳��Ă��Ȃ��Ȃ�Đ����Ȃ�

	AudioComponent[Index]->Sound = SoundList.GetData()[Index];//�T�E���h�ݒ�
	AudioComponent[Index]->Play();//�T�E���h���Đ�����
}

void ASoundManager::Play3DSound(int Index,UWorld* world,FVector vector)
{
	if (Index < 0 || Index >= SoundList.Num()) return;//���X�g�ɑ��݂��Ȃ��C���f�b�N�X�Ȃ�Đ����Ȃ�
	if (SoundList.GetData()[Index] == nullptr) return;//�T�E���h���ݒ肳��Ă��Ȃ��Ȃ�Đ����Ȃ�

	UGameplayStatics::PlaySoundAtLocation(world, SoundList.GetData()[Index], vector);
}

void ASoundManager::StopSound(int Index) {
	if (Index < 0 || Index >= SoundList.Num()) return;//���X�g�ɑ��݂��Ȃ��C���f�b�N�X�Ȃ�Đ����Ȃ�
	if (SoundList.GetData()[Index] == nullptr) return;//�T�E���h���ݒ肳��Ă��Ȃ��Ȃ�Đ����Ȃ�
	AudioComponent[Index]->Stop();//�T�E���h���~����
}


ASoundManager* ASoundManager::GetInstance()
{
	return SoundManagerInstance;
}

void ASoundManager::SafePlaySound(int Index)
{
	if (!SoundManagerInstance) return;
	SoundManagerInstance->PlaySound(Index);
}

void ASoundManager::SafePlay3DSound(int Index,UWorld* world, FVector vector)
{
	if (!SoundManagerInstance) return;
	SoundManagerInstance->Play3DSound(Index,world,vector);
}

void ASoundManager::SafeStopSound(int Index)
{
	if (!SoundManagerInstance) return;
	SoundManagerInstance->StopSound(Index);
}
