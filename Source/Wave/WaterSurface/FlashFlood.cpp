// Fill out your copyright notice in the Description page of Project Settings.


#include "FlashFlood.h"
#include "Runtime/Engine/Classes/Kismet//GameplayStatics.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"


// Sets default values
AFlashFlood::AFlashFlood()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFlashFlood::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AFlashFlood::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// ���Ԃ̌v��
	CurrentTime += (DeltaTime * Speed);

	//FVector Pos = GetActorLocation();
	//float XOffset = GetActorScale().X * 0.5f * ScaleInterpolation;
	//float YOffset = GetActorScale().Y * 0.5f * ScaleInterpolation;
	//FVector X_Vector = GetActorForwardVector() * XOffset;
	//FVector Y_Vector = GetActorRightVector() * YOffset;
	//// �f�o�b�O�p�̎l�p��`��(�����⑾���͌��ߑł��j
	//UKismetSystemLibrary::DrawDebugLine(this, Pos + X_Vector + Y_Vector, Pos + X_Vector - Y_Vector, FLinearColor::Blue, 0.0f, 3.0f);
	//UKismetSystemLibrary::DrawDebugLine(this, Pos + X_Vector - Y_Vector, Pos - X_Vector - Y_Vector, FLinearColor::Blue, 0.0f, 3.0f);
	//UKismetSystemLibrary::DrawDebugLine(this, Pos - X_Vector - Y_Vector, Pos - X_Vector + Y_Vector, FLinearColor::Blue, 0.0f, 3.0f);
	//UKismetSystemLibrary::DrawDebugLine(this, Pos - X_Vector + Y_Vector, Pos + X_Vector + Y_Vector, FLinearColor::Blue, 0.0f, 3.0f);
}

bool AFlashFlood::ShouldTickIfViewportsOnly() const
{
	return true;
}

bool AFlashFlood::CheckRange(FVector worldPos)
{
	FVector2D OtherPos = (FVector2D)worldPos;
	FVector2D Pos = (FVector2D)GetActorLocation();
	float XOffset = GetActorScale().X * 0.5f * ScaleInterpolation;
	float YOffset = GetActorScale().Y * 0.5f * ScaleInterpolation;
	FVector2D X_Vector = (FVector2D)GetActorForwardVector() * XOffset;
	FVector2D Y_Vector = (FVector2D)GetActorRightVector() * YOffset;
	FVector2D A = Pos + X_Vector + Y_Vector;
	FVector2D B = Pos + X_Vector - Y_Vector;
	FVector2D C = Pos - X_Vector - Y_Vector;
	FVector2D D = Pos - X_Vector + Y_Vector;

	if (FVector2D::CrossProduct(B - A, OtherPos - A) > 0) return false;
	if (FVector2D::CrossProduct(C - B, OtherPos - B) > 0) return false;
	if (FVector2D::CrossProduct(D - C, OtherPos - C) > 0) return false;
	if (FVector2D::CrossProduct(A - D, OtherPos - D) > 0) return false;

	// �S�Ă̊O�ς��}�C�i�X�̎��ɔ͈͓����ƕ�����
	return true;
}

FVector AFlashFlood::GetFloatVec(FVector worldPos)
{
	float Percent = GetHeight(worldPos) / MaxHight;
	return CheckRange(worldPos) ? GetFloatVector() * Speed * Power * Percent : FVector::ZeroVector;
}

FVector AFlashFlood::GetMinPos() const
{
	FVector2D Pos = (FVector2D)GetActorLocation();
	float XOffset = GetActorScale().X * 0.5f * ScaleInterpolation;
	float YOffset = GetActorScale().Y * 0.5f * ScaleInterpolation;
	FVector2D X_Vector = (FVector2D)GetActorForwardVector() * XOffset;
	FVector2D Y_Vector = (FVector2D)GetActorRightVector() * YOffset;
	FVector2D A = Pos + X_Vector + Y_Vector;
	FVector2D B = Pos + X_Vector - Y_Vector;
	FVector2D C = Pos - X_Vector - Y_Vector;
	FVector2D D = Pos - X_Vector + Y_Vector;

	if (Pos.X > A.X) Pos.X = A.X;
	if (Pos.X > B.X) Pos.X = B.X;
	if (Pos.X > C.X) Pos.X = C.X;
	if (Pos.X > D.X) Pos.X = D.X;

	if (Pos.Y > A.Y) Pos.Y = A.Y;
	if (Pos.Y > B.Y) Pos.Y = B.Y;
	if (Pos.Y > C.Y) Pos.Y = C.Y;
	if (Pos.Y > D.Y) Pos.Y = D.Y;

	return FVector(Pos, 0.0f);
}

FVector AFlashFlood::GetMaxPos() const
{
	FVector2D Pos = (FVector2D)GetActorLocation();
	float XOffset = GetActorScale().X * 0.5f * ScaleInterpolation;
	float YOffset = GetActorScale().Y * 0.5f * ScaleInterpolation;
	FVector2D X_Vector = (FVector2D)GetActorForwardVector() * XOffset;
	FVector2D Y_Vector = (FVector2D)GetActorRightVector() * YOffset;
	FVector2D A = Pos + X_Vector + Y_Vector;
	FVector2D B = Pos + X_Vector - Y_Vector;
	FVector2D C = Pos - X_Vector - Y_Vector;
	FVector2D D = Pos - X_Vector + Y_Vector;

	if (Pos.X < A.X) Pos.X = A.X;
	if (Pos.X < B.X) Pos.X = B.X;
	if (Pos.X < C.X) Pos.X = C.X;
	if (Pos.X < D.X) Pos.X = D.X;
			  
	if (Pos.Y < A.Y) Pos.Y = A.Y;
	if (Pos.Y < B.Y) Pos.Y = B.Y;
	if (Pos.Y < C.Y) Pos.Y = C.Y;
	if (Pos.Y < D.Y) Pos.Y = D.Y;

	return FVector(Pos, 0.0f);
}

float AFlashFlood::GetHeight(const FVector & worldPos)
{
	// ����̕����Ƌ����̓��ς��痬������̒��S�_����̋������擾
	FVector2D DistanceVec = FVector2D(GetActorLocation() - worldPos);
	float DotDistance = FVector2D::DotProduct((FVector2D)GetFloatVector(), DistanceVec);

	// ��������̋����ƌo�ߎ��Ԃ��獂���𐳌��g�̌v�Z�ŎZ�o
	float Radian = DotDistance * 2 * PI * SwingWeight * 0.001f + CurrentTime; // SwingWeight�̌��̒l�𒲐����₷�����邽�� 0.001f ���|����

	return FMath::Clamp(MaxHight * (float)sin(Radian), 0.0f, MaxHight);
}
