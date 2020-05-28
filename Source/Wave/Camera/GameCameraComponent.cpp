// Fill out your copyright notice in the Description page of Project Settings.

#include "GameCameraComponent.h"
#include "Camera/CameraComponent.h"
#include "../InputManager.h"
#include "../MyFunc.h"

// Sets default values for this component's properties
UGameCameraComponent::UGameCameraComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bAutoActivate = true;
	bTickInEditor = true;
}

void UGameCameraComponent::BeginPlay()
{
	Super::BeginPlay();

	if (FollowCamera)
	{
		// �����̃J���������ŏ�����
		FollowDistance = FollowCamera->GetRelativeLocation().Z;
	}
}

void UGameCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateRotation();

	// CameraBoom�̃��[�J����Ԃŋ�����ݒ�
	if (FollowCamera)
	{
		// �Ȃ߂炩�ɋ�����ύX
		FollowDistance = FMath::Lerp<float>(FollowDistance, TargetFollowDistance, 0.1f);
		FollowCamera->SetRelativeLocation(FVector(-FollowDistance, 0.0f, 0.0f));
	}
}

void UGameCameraComponent::UpdateRotation()
{
	const AInputManager * inputManager = AInputManager::GetInstance();
	if (!inputManager) return;
	const InputState * input = inputManager->GetState();

	FQuat NewQuat;
	FQuat AddHorizontal, AddVertical;
	// ���[���h�n�̉�]���擾
	NewQuat = GetRelativeRotationFromWorld(GetRelativeRotation().Quaternion());

	// ���ꂼ��̎��̉�]�ʂ��v�Z
	AddVertical = MyFunc::FromAxisAngleToQuaternion(NewQuat.GetRightVector(), input->RightStick.Vertical * 0.01f * RotationSpeed).GetNormalized();
	AddHorizontal = MyFunc::FromAxisAngleToQuaternion(FVector::UpVector, -input->RightStick.Horizontal * 0.01f * RotationSpeed).GetNormalized();
	// ��]�����Z����
	NewQuat = AddVertical * NewQuat;	// �Ɉʕ������Ɍv�Z
	NewQuat = AddHorizontal * NewQuat;

	// �Ɉʕ����̊p�x���������`�F�b�N
	if (CheckVerticalLimit(NewQuat, AddVertical))
	{
		NewQuat = AddVertical * NewQuat;
	}
	NewQuat.Normalize();

	// �ŏI�I�ȉ�]����
	SetWorldRotation(NewQuat);

	// �q�̃g�����X�t�H�[�����X�V
	UpdateChildTransforms();
}

bool UGameCameraComponent::CheckVerticalLimit(const FQuat & CheckQuat, FQuat & OutOverQuat)
{
	FVector ForwardVec = CheckQuat.GetForwardVector();
	FVector StartForwardVec = ForwardVec;
	StartForwardVec.Z = 0.0f; StartForwardVec.Normalize();
	if (CheckQuat.GetUpVector().Z < 0.0f) StartForwardVec = -StartForwardVec;	// �J�������t���ɂȂ��Ă�����x�N�g�����]

	float DotF = FVector::DotProduct(StartForwardVec, ForwardVec);
	bool Up_OR_Down = (FVector::DotProduct(FVector::UpVector, ForwardVec) < 0);	// �ォ����

	float DegreeAngle = FMath::RadiansToDegrees(FMath::Acos(DotF));	// �p�x�Z�o
	if (Up_OR_Down)
	{
		DegreeAngle = VerticalMax - DegreeAngle;
		if (DegreeAngle > 0.0f) return false;
	}
	else
	{
		DegreeAngle = VerticalMin + DegreeAngle;
		if (DegreeAngle <= 0.0f) return false;
	}

	// �p�x�Z�o���ăI�[�o�[����������
	OutOverQuat = MyFunc::FromAxisAngleToQuaternion(CheckQuat.GetRightVector(), FMath::DegreesToRadians(DegreeAngle)).GetNormalized();
	return true;
}

