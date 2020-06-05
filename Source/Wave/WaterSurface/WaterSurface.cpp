// �g�̉��Z�ɂ͔g�����������g�p���Ă���
// �ȉ��g���������̎Q�l�T�C�g
// https://qiita.com/Ushio/items/0249fd7a5363ccd914dd
// https://www.slideshare.net/UnityTechnologiesJapan/unite-2017-tokyo3d-76689196
// https://www.nicovideo.jp/watch/sm9470923

#include "WaterSurface.h"
#include "Kismet/GameplayStatics.h"
#include "CircleLand.h"
#include "SquareLand.h"
#include "FlashFlood.h"
#include "FloatActor.h"
#include "BreakSquareLand.h"
#include "../MyFunc.h"

AWaterSurface::AWaterSurface() : AProceduralMeshActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWaterSurface::BeginPlay()
{
	Super::BeginPlay();

	SetActorTransform(FTransform::Identity);

	// ���_�̐����t�B�[���h�̑傫���ɉ����Č��肷��
	float X_Start = StartPoint->GetActorLocation().X;
	float Y_Start = StartPoint->GetActorLocation().Y;
	SplitPointNum.X = (int)((-X_Start + EndPoint->GetActorLocation().X) / SplitSpace);
	SplitPointNum.Y = (int)((-Y_Start + EndPoint->GetActorLocation().Y) / SplitSpace);

	for (int yi = 0; yi < SplitPointNum.Y; ++yi)
	{
		for (int xi = 0; xi < SplitPointNum.X; ++xi)
		{
			Vertices.Emplace(SplitSpace * xi + X_Start, SplitSpace * yi + Y_Start, 0);
			VertexColors.Emplace(WaterColor);
			UV0.Emplace((xi / SplitPointNum.X) * 0.5f, (yi / SplitPointNum.Y));

			if (xi < SplitPointNum.X - 1 && yi < SplitPointNum.Y - 1)
			{
				Triangles.Emplace(CalcIndex(xi + 0, yi + 0));
				Triangles.Emplace(CalcIndex(xi + 0, yi + 1));
				Triangles.Emplace(CalcIndex(xi + 1, yi + 0));
				Triangles.Emplace(CalcIndex(xi + 1, yi + 0));
				Triangles.Emplace(CalcIndex(xi + 0, yi + 1));
				Triangles.Emplace(CalcIndex(xi + 1, yi + 1));
			}
		}
	}

	VertexTypes.Init(VertexType::Water, Vertices.Num());

	// �����o�̒��_�f�[�^�Ȃǂ��烁�b�V���𐶐�
	CreateMesh();

	// �g���Z�p���X�g�̏�����
	CurrentHeights.Init(0.0f, SplitPointNum.X * SplitPointNum.Y);
	PrevHeights.Init(0.0f, SplitPointNum.X * SplitPointNum.Y);
	NewHeights.Init(0.0f, SplitPointNum.X * SplitPointNum.Y);

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALandPoint::StaticClass(), FoundActors);
	
	// ���߂ɒn�ʂ������Z�b�g
	for (auto Actor : FoundActors)
	{
		// ���\�����Ɏg���̂Ń����o�ɓo�^���Ă���
		ALandPoint * LandPoint = Cast<ALandPoint>(Actor);
		LandPointActors.Add(LandPoint);
		if (!LandPoint->IsLand) continue;

		// �~�`�̒n�`�̏�����
		ACircleLand* CircleLand = Cast<ACircleLand>(Actor);
		if (CircleLand)
		{
			SetCircleLand(CircleLand->GetActorLocation(), CircleLand->GetRadius(), VertexType::Land);
			continue;
		}
		// ��`�̒n�`�̏�����
		ASquareLand* SquareLand = Cast<ASquareLand>(Actor);
		if (SquareLand)
		{
			SetSquareLand(SquareLand->GetActorLocation(), SquareLand->GetXLength(), SquareLand->GetYLength(), VertexType::Land);
		}
	}
	// ���ɊR���Z�b�g
	for (auto Actor : LandPointActors)
	{
		if (Actor->IsLand) continue;
		// �~�`�̒n�`�̏�����
		ACircleLand* CircleLand = Cast<ACircleLand>(Actor);
		if (CircleLand)
		{
			SetCircleLand(CircleLand->GetActorLocation(), CircleLand->GetRadius(), VertexType::Cliff);
			continue;
		}
		// ��`�̒n�`�̏�����
		ASquareLand* SquareLand = Cast<ASquareLand>(Actor);
		if (SquareLand)
		{
			SetSquareLand(SquareLand->GetActorLocation(), SquareLand->GetXLength(), SquareLand->GetYLength(), VertexType::Cliff);
		}
	}

	TArray<AActor*> FoundFlashFloods;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFlashFlood::StaticClass(), FoundFlashFloods);
	for (auto Actor : FoundFlashFloods)
	{
		// ���\�����Ɏg���̂Ń����o�ɓo�^���Ă���
		FlashFloods.Add(Cast<AFlashFlood>(Actor));
	}


	TArray<AActor*> FoundBreakLand;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABreakSquareLand::StaticClass(), FoundFlashFloods);
	for (auto Actor : FoundFlashFloods)
	{
		// ���\�����Ɏg���̂Ń����o�ɓo�^���Ă���
		FoundBreakLand.Add(Cast<ABreakSquareLand>(Actor));
	}


	if (Material)
	{
		Mesh->SetMaterial(0, Material);
	}

	UpdateMesh();
}

void AWaterSurface::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float DeltaSpeed = WaveSpeed / 60;	

	float c = 2.0f;
	float mul = DeltaSpeed * DeltaSpeed * c * c / (SplitPointNum.X * SplitPointNum.Y) * (SplitPointNum.X * SplitPointNum.Y);
	for (int xi = 1; xi < SplitPointNum.X - 1; ++xi)
	{
		for (int yi = 1; yi < SplitPointNum.Y - 1; ++yi)
		{
			int32 index = CalcIndex(xi, yi);

			float uL = CurrentHeights[this->CalcIndex(xi - 1, yi)];
			float uR = CurrentHeights[this->CalcIndex(xi + 1, yi)];
			float uT = CurrentHeights[this->CalcIndex(xi, yi - 1)];
			float uB = CurrentHeights[this->CalcIndex(xi, yi + 1)];

			float u_pre = PrevHeights[index];
			float u = CurrentHeights[index];

			float damp = Decay * DeltaSpeed * (u - u_pre);

			NewHeights[index] = u + u - u_pre + (mul * (uL + uR + uT + uB - (4.0f * u))) - damp;
		}
	}

	for (int xi = 1; xi < SplitPointNum.X - 1; ++xi)
	{
		for (int yi = 1; yi < SplitPointNum.Y - 1; ++yi)
		{
			int32 index = CalcIndex(xi, yi);
			if (VertexTypes[index] != VertexType::Water/* ||
				abs(NewHeights[index]) > abs(CurrentHeights[index])*/)
				continue;

			PrevHeights[index] = CurrentHeights[index];
			CurrentHeights[index] = NewHeights[index];

			//if (VertexTypes[index] == VertexType::Water)
			{
				Vertices[index].Z = CurrentHeights[index];
				// �����ɉ����ăJ���[��ύX
				float Progress = FMath::Clamp(FMath::Abs(Vertices[index].Z) / (MaxWaveHight * 0.5f), 0.0f, 1.0f);
				VertexColors[index] = FLinearColor::LerpUsingHSV(WaterColor, WaveColor, Progress);
				// ����������K�p
				Vertices[index].Z = FMath::Clamp(Vertices[index].Z, -MaxWaveHight, MaxWaveHight);
			}
		}
	}

	// ���ʂ̔g�ɂȂ�Ȃ��悤�����͍Ō�Ɍv�Z
	for (auto Actor : FlashFloods)
	{
		TickFlashFloodWave(Actor);
	}

	UpdateMesh();
}

void AWaterSurface::CreateWave(int32 x, int32 y, float power)
{
	// �K�E�X���Z
	auto gauss = [](float distance, float sigma)
	{
		return 1.0f / sqrt(PI * 2) * sigma * exp(-distance * distance / (2.0f * sigma * sigma));
	};

	FVector2D wv = FVector2D(x, y);

	for (int xi = 1; xi < SplitPointNum.X - 1; ++xi)
	{
		for (int yi = 1; yi < SplitPointNum.Y - 1; ++yi)
		{
			int index = CalcIndex(xi, yi);
			if (VertexTypes[index] != VertexType::Water) continue;
			float value = 0.0f;

			FVector2D v = FVector2D(xi, yi);

			float norm = FVector2D::Distance(v, wv);
			value += gauss(norm, 3.0f) * power;

			CurrentHeights[index] += value;
			PrevHeights[index] += value;
			NewHeights[index] += value;
		}
	}
}

int32 AWaterSurface::CalcIndex(int32 x, int32 y)
{
	int32 index = x + (y * SplitPointNum.X);
	if (index < 0 ) return 0;
	if (index > SplitPointNum.X * SplitPointNum.Y) return 0;
	return index;
}

void AWaterSurface::SetCircleLand(FVector CirclePostion, float Radius, VertexType Type)
{
	for (int xi = 0; xi < SplitPointNum.X; ++xi)
	{
		for (int yi = 0; yi < SplitPointNum.Y; ++yi)
		{
			int32 index = CalcIndex(xi, yi);
			float xp = Vertices[index].X;
			float yp = Vertices[index].Y;
			float xc = CirclePostion.X;
			float yc = CirclePostion.Y;
			if ((xp - xc)*(xp - xc) + (yp - yc)*(yp - yc) <= Radius * Radius)
			{
				SetLand(xi, yi, CirclePostion.Z, Type);
			}
		}
	}
}

void AWaterSurface::SetSquareLand(FVector SquareLocation, float XLength, float YLength, VertexType Type)
{
	for (int xi = 0; xi < SplitPointNum.X; ++xi)
	{
		for (int yi = 0; yi < SplitPointNum.Y; ++yi)
		{
			int32 index = CalcIndex(xi, yi);
			float xp = Vertices[index].X;
			float yp = Vertices[index].Y;
			float xs = SquareLocation.X;
			float ys = SquareLocation.Y;

			if (xp > xs + XLength * 0.5f) continue;
			if (xp < xs - XLength * 0.5f) continue;
			if (yp > ys + YLength * 0.5f) continue;
			if (yp < ys - YLength * 0.5f) continue;

			SetLand(xi, yi, SquareLocation.Z, Type);
		}
	}
}

void AWaterSurface::SetLand(int X, int Y, float Z, VertexType Type)
{
	int32 index = CalcIndex(X, Y);
	switch (Type)
	{
	case VertexType::Water:
		VertexTypes[index] = VertexType::Water;
		Vertices[index].Z = Z;
		VertexColors[index] = (WaterColor);
		UV0[index] = FVector2D((X / SplitPointNum.X) * 0.5f, (Y / SplitPointNum.Y));
		break;
	case VertexType::Land:
		Vertices[index].Z = Z;
		VertexColors[index] = FLinearColor::Black;
		VertexTypes[index] = VertexType::Land;
		UV0[index] = FVector2D((X / SplitPointNum.X) * 0.5f + 0.5f, (Y / SplitPointNum.Y));
		break;
	case VertexType::Cliff:
		// ���ɊR�����ɐݒ�ς݂Ȃ牽�����Ȃ�
		if (VertexTypes[index] != VertexType::Water) return;

		FLinearColor CliffColor = WaterColor + FLinearColor::White * 0.03f;
		Vertices[index].Z = -10000.0f;
		VertexTypes[index] = VertexType::Cliff;
		VertexColors[index] = CliffColor;

		// ���͂̐F���ω�������
		VertexColors[CalcIndex(X + 1, Y + 0)] = CliffColor;
		VertexColors[CalcIndex(X + 1, Y + 1)] = CliffColor;
		VertexColors[CalcIndex(X + 1, Y - 1)] = CliffColor;
		VertexColors[CalcIndex(X - 1, Y + 0)] = CliffColor;
		VertexColors[CalcIndex(X - 1, Y + 1)] = CliffColor;
		VertexColors[CalcIndex(X - 1, Y - 1)] = CliffColor;
		VertexColors[CalcIndex(X + 0, Y - 1)] = CliffColor;
		VertexColors[CalcIndex(X + 0, Y + 1)] = CliffColor;
		UV0[index] = FVector2D((X / SplitPointNum.X) * 0.5f + 0.5f, (Y / SplitPointNum.Y));
		break;
	}
}

void AWaterSurface::TickFlashFloodWave(AFlashFlood* FlashFlood)
{
	for (int xi = 1; xi < SplitPointNum.X - 1; ++xi)
	{
		for (int yi = 1; yi < SplitPointNum.Y - 1; ++yi)
		{
			int index = CalcIndex(xi, yi);

			if (VertexTypes[index] != VertexType::Water) continue;
			if (!FlashFlood->CheckRange(Vertices[index])) continue;

			float NewHight = FlashFlood->GetHeight(Vertices[index]);

			// �g�Ɛ��������̉e��������ꍇ�A�����̍�������D��
			//Vertices[index].Z = Vertices[index].Z < NewHight ? NewHight : Vertices[index].Z;

			// ���Z���������ʔ����̂ł������I
			Vertices[index].Z = Vertices[index].Z + NewHight;

			// ����������K�p
			Vertices[index].Z = FMath::Clamp(Vertices[index].Z, -MaxWaveHight, MaxWaveHight);

			// �����ɉ����ăJ���[��ύX
			VertexColors[index] = FLinearColor::LerpUsingHSV(WaterColor, WaveColor, FMath::Abs(Vertices[index].Z) / MaxWaveHight);
		}
	}
}

void AWaterSurface::AddPower(FVector worldPos, float power)
{
	int32 WaveX = (worldPos.X - Vertices[0].X) / SplitSpace;
	int32 WaveY = (worldPos.Y - Vertices[0].Y) / SplitSpace;
	float HeightPower = FMath::Abs(worldPos.Z);
	HeightPower = (HeightPower > MaxWaveHight) ? 0.0f : (MaxWaveHight - HeightPower) / MaxWaveHight;
	
	HammerBreakLand(worldPos);

	CreateWave(WaveX, WaveY, power * HeightPower);
}

void AWaterSurface::HammerBreakLand(const FVector & worldPos)
{
	ABreakSquareLand* BreakLand = Cast<ABreakSquareLand>(GetLandPoint(worldPos, true));
	if (!BreakLand)return;
	BreakLand->Break();
}

float AWaterSurface::GetWaveHeight(const FVector & worldPos)
{
	if (!IsInWater(worldPos)) return 0.0f;

	int32 WaveX = (worldPos.X - Vertices[0].X) / SplitSpace;
	int32 WaveY = (worldPos.Y - Vertices[0].Y) / SplitSpace;
	float uL = 0.0f, uR = 0.0f, uT = 0.0f, uB = 0.0f;

	if (WaveX >= 0 && WaveX < SplitPointNum.X && WaveY >= 0 && WaveY < SplitPointNum.Y)
	{
		uL = Vertices[CalcIndex(WaveX - 1, WaveY)].Z;
		uR = Vertices[CalcIndex(WaveX + 1, WaveY)].Z;
		uT = Vertices[CalcIndex(WaveX, WaveY - 1)].Z;
		uB = Vertices[CalcIndex(WaveX, WaveY + 1)].Z;
	}

	float result = (uL + uR + uT + uB) * 0.25f;	// �����̕��ς�Ԃ�
	return (result < MaxWaveHight) ? result : MaxWaveHight;	
}

FVector AWaterSurface::GetWavePower(const FVector & worldPos)
{
	if (!IsInWater(worldPos)) return FVector::ZeroVector;

	FVector answerVec = FVector::ZeroVector;

	for (auto Actor : FlashFloods)
	{
		FVector addVec = Actor->GetFloatVec(worldPos);
		addVec.Z = 0;
		answerVec += addVec;
	}

	int32 WaveX = (worldPos.X - Vertices[0].X) / SplitSpace;
	int32 WaveY = (worldPos.Y - Vertices[0].Y) / SplitSpace;
	float uL = 0.0f, uR = 0.0f, uT = 0.0f, uB = 0.0f;
	float iL = 0.0f, iR = 0.0f, iT = 0.0f, iB = 0.0f;
	
	if (WaveX >= 0 && WaveX < SplitPointNum.X && WaveY >= 0 && WaveY < SplitPointNum.Y)
	{
		uL = (VertexTypes[CalcIndex(WaveX - 1, WaveY)] == VertexType::Water) ? Vertices[CalcIndex(WaveX - 1, WaveY)].Z : 0.0f;
		uR = (VertexTypes[CalcIndex(WaveX + 1, WaveY)] == VertexType::Water) ? Vertices[CalcIndex(WaveX + 1, WaveY)].Z : 0.0f;
		uT = (VertexTypes[CalcIndex(WaveX, WaveY - 1)] == VertexType::Water) ? Vertices[CalcIndex(WaveX, WaveY - 1)].Z : 0.0f;
		uB = (VertexTypes[CalcIndex(WaveX, WaveY + 1)] == VertexType::Water) ? Vertices[CalcIndex(WaveX, WaveY + 1)].Z : 0.0f;

		iL = (VertexTypes[CalcIndex(WaveX - 1, WaveY - 1)] == VertexType::Water) ? Vertices[CalcIndex(WaveX - 1, WaveY - 1)].Z : 0.0f;
		iR = (VertexTypes[CalcIndex(WaveX + 1, WaveY + 1)] == VertexType::Water) ? Vertices[CalcIndex(WaveX + 1, WaveY + 1)].Z : 0.0f;
		iT = (VertexTypes[CalcIndex(WaveX + 1, WaveY - 1)] == VertexType::Water) ? Vertices[CalcIndex(WaveX + 1, WaveY - 1)].Z : 0.0f;
		iB = (VertexTypes[CalcIndex(WaveX - 1, WaveY + 1)] == VertexType::Water) ? Vertices[CalcIndex(WaveX - 1, WaveY + 1)].Z : 0.0f;
	}
	float HeightValue = 0.0f;
	/*HeightValue = FMath::Max(HeightValue, (uL));
	HeightValue = FMath::Max(HeightValue, (uR));
	HeightValue = FMath::Max(HeightValue, (uT));
	HeightValue = FMath::Max(HeightValue, (uB));

	HeightValue = FMath::Max(HeightValue, (iL));
	HeightValue = FMath::Max(HeightValue, (iR));
	HeightValue = FMath::Max(HeightValue, (iT));
	HeightValue = FMath::Max(HeightValue, (iB));*/

	HeightValue = (abs(uL) + abs(uR) + abs(uT) + abs(uB) + abs(iL) + abs(iR) + abs(iT) + abs(iB)) / 8;

	float x = uL - uR;
	float y = uT - uB;

	answerVec += FVector(x, y, 0).GetSafeNormal() * (HeightValue / MaxWaveHight) * WavePower;

	return answerVec;
}

FVector AWaterSurface::AdjustMoveInField(const FVector & worldPos, float circleRadius)
{
	FVector Result = worldPos;

	float Right_X = Vertices[Vertices.Num() - 1].X;
	float Left_X = Vertices[0].X;
	float Up_Y = Vertices[Vertices.Num() - 1].Y;
	float Bottom_Y = Vertices[0].Y;

	// X�����m�F
	float Deff = Result.X + circleRadius - (Right_X);
	if (Deff > 0.0f) Result.X -= Deff;
	else
	{
		Deff = Result.X - circleRadius - (Left_X);
		if (Deff < 0.0f) Result.X -= Deff;
	}
	// Y�����m�F
	Deff = Result.Y + circleRadius - (Up_Y);
	if (Deff > 0.0f) Result.Y -= Deff;
	else
	{
		Deff = Result.Y - circleRadius - (Bottom_Y);
		if (Deff < 0.0f) Result.Y -= Deff;
	}

	return Result;
}

// ���̍��W�𒲂ׂĈړ�����ʒu���Փ˂��Ă����牟���o���֐�
FVector AWaterSurface::AdjustMoveInLand(const FVector & worldPos, const FVector & moveVec, float circleRadius, const FVector & WaterCheckPos, float WaterCheckRadius)
{
	FVector movedPos = worldPos + moveVec;

	// �t�B�[���h�̊O�ɏo�Ȃ��悤�ɂ���
	movedPos = AdjustMoveInField(movedPos, circleRadius);

	// �ړ��悪�v���C���[�����邾���̒n�ʂ�����Έړ�����
	if (GetLandPointInside(movedPos, circleRadius) != nullptr) 
		return movedPos;

	// �t�B�[���h�̃��b�V���S�̂�����
	for (int xi = 0; xi < SplitPointNum.X; ++xi)
	{
		for (int yi = 0; yi < SplitPointNum.Y; ++yi)
		{
			int index = CalcIndex(xi, yi);
			if (VertexTypes[index] == VertexType::Land) continue;	// �n��Ȃ狗���𑪂�K�v���Ȃ��̂Ŏ���

			float xp = Vertices[index].X;
			float yp = Vertices[index].Y;
			float xc = WaterCheckPos.X;
			float yc = WaterCheckPos.Y;

			// �ړ���̍��W�Ɛ��Ƃ̋����𑪂�
			if ((xp - xc)*(xp - xc) + (yp - yc)*(yp - yc) <= WaterCheckRadius * WaterCheckRadius)
			{	
				// �����ňړ��悪���ƕ��������̂ō�����n�ʂ𒲂ׂ�
				// �n�`�̌`�ɂ���Ă��ꂼ�ꏈ��������
				ALandPoint * LandActor = GetLandPoint(worldPos);

				// ���ɐ��̏�ɑ��݂��Ă��鎞�͂��̂܂܈ړ�����
				if (!LandActor) return movedPos;

				return LandActor->AdjustMoveInLand(movedPos, circleRadius);
			}
		}
	}

	// �ړ��悪�n�ゾ�����̂ł��̂܂܈ړ�
	return movedPos;
}

FVector AWaterSurface::AdjustMoveInWater(const AActor * Object, FVector& moveVec, float circleRadius)
{
	FVector actorPos = Object->GetActorLocation();
	FVector movedPos = actorPos + moveVec;

	TArray<AActor*> FoundActors;
	// ����FloatActor�Ƃ̓����蔻��
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFloatActor::StaticClass(), FoundActors);
	for (auto Actor : FoundActors)
	{
		AFloatActor* FloatActor = Cast<AFloatActor>(Actor);
		if (!FloatActor || Object == Actor) continue;

		movedPos = FloatActor->AdjustMove_VS_Circle(actorPos, movedPos, moveVec, circleRadius);
	}

	// �n�`�Ɠ����蔻��
	for (auto Actor : LandPointActors)
	{
		if (!Actor->IsLand) continue;

		movedPos = Actor->AdjustMoveOutWater(actorPos, movedPos, moveVec, circleRadius);
	}
	
	return movedPos;
}

FVector AWaterSurface::AdjustMoveInWater(const AActor * Object, FVector & moveVec, float XLen, float YLen)
{
	FVector actorPos = Object->GetActorLocation();
	FVector movedPos = actorPos + moveVec;

	TArray<AActor*> FoundActors;
	// ����FloatActor�Ƃ̓����蔻��
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFloatActor::StaticClass(), FoundActors);
	for (auto Actor : FoundActors)
	{
		AFloatActor* FloatActor = Cast<AFloatActor>(Actor);
		if (!FloatActor || Object == Actor) continue;

		movedPos = FloatActor->AdjustMove_VS_Square(actorPos, movedPos, moveVec, XLen, YLen);
	}

	// �n�`�Ɠ����蔻��
	for (auto Actor : LandPointActors)
	{
		if (!Actor->IsLand) continue;
		movedPos = Actor->AdjustMoveOutWater(actorPos, movedPos, moveVec, XLen, YLen);
	}

	return movedPos;
}

bool AWaterSurface::IsInWater(FVector worldPos)
{
	if (!IsInField(worldPos)) return false;

	int32 WaveX = (worldPos.X - Vertices[0].X) / SplitSpace;
	int32 WaveY = (worldPos.Y - Vertices[0].Y) / SplitSpace;

	int index = CalcIndex(WaveX, WaveY);

	if (index <= 1) return false;
	if (index >= Vertices.Num() - 1) return false;

	return VertexTypes[index] == VertexType::Water;
}

bool AWaterSurface::IsInLand(FVector worldPos)
{
	if (!IsInField(worldPos)) return false;

	int32 WaveX = (worldPos.X - Vertices[0].X) / SplitSpace;
	int32 WaveY = (worldPos.Y - Vertices[0].Y) / SplitSpace;

	int index = CalcIndex(WaveX, WaveY);

	if (index <= 1) return false;
	if (index >= Vertices.Num() - 1) return false;

	return VertexTypes[index] == VertexType::Land;
}

VertexType AWaterSurface::GetVertexType(FVector worldPos)
{
	int32 WaveX = (worldPos.X - Vertices[0].X) / SplitSpace;
	int32 WaveY = (worldPos.Y - Vertices[0].Y) / SplitSpace;

	int index = CalcIndex(WaveX, WaveY);

	if (index >= Vertices.Num()) return VertexType::None;

	return VertexTypes[index];
}

bool AWaterSurface::IsInField(FVector worldPos)
{
	VertexType type = GetVertexType(worldPos);
	if (type == VertexType::Cliff ||
		type == VertexType::None)
		return false;

	FVector2D Origin = FVector2D(Vertices[0].X, Vertices[0].Y);
	FVector2D End = FVector2D(Vertices[Vertices.Num() - 1].X, Vertices[Vertices.Num() - 1].Y);

	return Origin.X <= worldPos.X && End.X >= worldPos.X &&
		   Origin.Y <= worldPos.Y && End.Y >= worldPos.Y;
}

bool AWaterSurface::IsInField(FVector worldPos, float CircleRadius)
{
	ALandPoint * CliffPoint = GetLandPoint(worldPos, false);
	if(CliffPoint && CliffPoint->InGround(worldPos, CircleRadius))
		return false;

	FVector2D Origin = FVector2D(Vertices[0].X, Vertices[0].Y);
	FVector2D End = FVector2D(Vertices[Vertices.Num() - 1].X, Vertices[Vertices.Num() - 1].Y);

	return Origin.X <= worldPos.X + CircleRadius && End.X >= worldPos.X - CircleRadius &&
		Origin.Y <= worldPos.Y + CircleRadius && End.Y >= worldPos.Y - CircleRadius;
}

FVector AWaterSurface::GetGetOffPos(FVector WorldPos, float Radius)
{
	for (int xi = 1; xi < SplitPointNum.X - 1; ++xi)
	{
		for (int yi = 1; yi < SplitPointNum.Y - 1; ++yi)
		{
			if (VertexTypes[CalcIndex(xi, yi)] != VertexType::Land) continue;

			float xp = Vertices[CalcIndex(xi, yi)].X;
			float yp = Vertices[CalcIndex(xi, yi)].Y;
			float xc = WorldPos.X;
			float yc = WorldPos.Y;
			if ((xp - xc)*(xp - xc) + (yp - yc)*(yp - yc) <= Radius * Radius)
			{
				return Vertices[CalcIndex(xi,yi)];
			}
		}
	}

	return FVector::ZeroVector;
}

// �����̍��W�̈ʒu�ɂ���n�ʂ��擾
ALandPoint * AWaterSurface::GetLandPoint(const FVector & WorldPos, bool IsLand)
{
	for (auto Actor : LandPointActors)
	{
		if (Actor->IsLand != IsLand) continue;

		if (Actor->OnGround(WorldPos))
		{
			return Actor;
		}
	}
	return nullptr;
}

ALandPoint * AWaterSurface::GetLandPoint(const FVector & WorldPos, float Radius, bool IsLand)
{
	for (auto Actor : LandPointActors)
	{
		if (Actor->IsLand != IsLand) continue;

		if (Actor->OnGround(WorldPos, Radius))
		{
			return Actor;
		}
	}
	return nullptr;
}

ALandPoint * AWaterSurface::GetLandPointInside(const FVector & WorldPos, float Radius, bool IsLand)
{
	for (auto Actor : LandPointActors)
	{
		if (Actor->IsLand != IsLand) continue;

		if (Actor->InGround(WorldPos, Radius))
		{
			return Actor;
		}
	}
	return nullptr;
}

FVector AWaterSurface::GetCenterPos()
{
	return (StartPoint->GetActorLocation() + EndPoint->GetActorLocation()) * 0.5f;
}
