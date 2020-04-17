#include "SoundManager.h"

//�C���X�^���X�ւ̃A�h���X
static ASoundManager* SoundManagerInstance = nullptr;

// Sets default values
ASoundManager::ASoundManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("ModifySkeletalMeshComponent"));
	SoundManagerInstance = this;
}

void ASoundManager::PlaySound(int Index)
{
	if (Index < 0 || Index >= SoundList.Num()) return;//���X�g�ɑ��݂��Ȃ��C���f�b�N�X�Ȃ�Đ����Ȃ�
	if (SoundList.GetData()[Index] == nullptr) return;//�T�E���h���ݒ肳��Ă��Ȃ��Ȃ�Đ����Ȃ�
	AudioComponent->Sound = SoundList.GetData()[Index];//�T�E���h�ݒ�
	AudioComponent->Play();//�T�E���h���Đ�����
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