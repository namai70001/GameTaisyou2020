// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ProceduralMeshActor.generated.h"

UCLASS()
class WAVE_API AProceduralMeshActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AProceduralMeshActor();

protected:
	virtual void BeginPlay() override;

	// ���b�V���̍쐬
	// ��ɒ��_���̂������̒l��ݒ肵�Ă���Ăяo��
	virtual void CreateMesh();

	virtual void UpdateMesh();

public:	
	virtual void Tick(float DeltaTime) override;
	
protected:
	UProceduralMeshComponent* Mesh;

	// ���_�Q
	TArray<FVector> Vertices;
	// �C���f�b�N�X�Q
	TArray<int32> Triangles;
	// �@���Q
	TArray<FVector> Normals;
	// �e�N�X�`���[���W�Q
	TArray<FVector2D> UV0;
	// ���_�J���[�Q
	TArray<FLinearColor> VertexColors;
	// �ڐ��Q
	TArray<FProcMeshTangent> Tangents;
	// �����蔻�萶���t���O
	bool IsCreateCollision = true;
};
