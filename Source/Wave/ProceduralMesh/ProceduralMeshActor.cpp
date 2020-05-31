// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralMeshActor.h"

// Sets default values
AProceduralMeshActor::AProceduralMeshActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// mesh �̒��g�����B
	Mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("generated-mesh"));
	// �Ƃ肠���� RootComponent �Ƃ��� mesh �������ށB
	RootComponent = Mesh;
}

// Called when the game starts or when spawned
void AProceduralMeshActor::BeginPlay()
{
	Super::BeginPlay();
}

void AProceduralMeshActor::CreateMesh()
{
	// UProceduralMeshComponent::CreateMeshSection_LinearColor �Ń��b�V���𐶐��B
	// ��1����: �Z�N�V�����ԍ�; 0, 1, 2, ... ��^���鎖��1�� UProceduralMeshComponent �ɕ����̃��b�V��������I�ɓ����ɐ����ł��܂��B
	// ��2����: ���_�Q
	// ��3����: �C���f�b�N�X�Q
	// ��4����: �@���Q
	// ��5����: �e�N�X�`���[���W�Q
	// ��6����: ���_�J���[�Q
	// ��7����: �@���Q
	// ��8����: �R���W���������t���O
	Mesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UV0, VertexColors, Tangents, IsCreateCollision);
}

void AProceduralMeshActor::UpdateMesh()
{
	Mesh->UpdateMeshSection_LinearColor(0, Vertices, Normals, UV0, VertexColors, Tangents);
}

// Called every frame
void AProceduralMeshActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

