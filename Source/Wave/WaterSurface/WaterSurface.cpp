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

	IsLands.Init(false, Vertices.Num());

	// �����o�̒��_�f�[�^�Ȃǂ��烁�b�V���𐶐�
	CreateMesh();

	// �g���Z�p���X�g�̏�����
	CurrentHeights.Init(0.0f, SplitPointNum.X * SplitPointNum.Y);
	PrevHeights.Init(0.0f, SplitPointNum.X * SplitPointNum.Y);
	NewHeights.Init(0.0f, SplitPointNum.X * SplitPointNum.Y);

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALandPoint::StaticClass(), FoundActors);
	
	for (auto Actor : FoundActors)
	{
		// ���\�����Ɏg���̂Ń����o�ɓo�^���Ă���
		LandPointActors.Add(Cast<ALandPoint>(Actor));

		// �~�`�̒n�`�̏�����
		ACircleLand* CircleLand = Cast<ACircleLand>(Actor);
		if (CircleLand)
		{
			SetCircleLand(CircleLand->GetActorLocation(), CircleLand->GetRadius());
			continue;
		}
		// ��`�̒n�`�̏�����
		ASquareLand* SquareLand = Cast<ASquareLand>(Actor);
		if (SquareLand)
		{
			SetSquareLand(SquareLand->GetActorLocation(), SquareLand->GetXLength(), SquareLand->GetYLength());
		}
	}

	TArray<AActor*> FoundFlashFloods;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFlashFlood::StaticClass(), FoundFlashFloods);
	for (auto Actor : FoundFlashFloods)
	{
		// ���\�����Ɏg���̂Ń����o�ɓo�^���Ă���
		FlashFloods.Add(Cast<AFlashFlood>(Actor));
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
			if (IsLands[index] && 
				abs(NewHeights[index]) > abs(CurrentHeights[index]))
				continue;

			PrevHeights[index] = CurrentHeights[index];
			CurrentHeights[index] = NewHeights[index];

			if (!IsLands[index])
			{
				Vertices[index].Z = CurrentHeights[index];
				// �����ɉ����ăJ���[��ύX
				VertexColors[index] = FLinearColor::LerpUsingHSV(WaterColor, WaveColor, FMath::Abs(Vertices[index].Z) / MaxWaveHight);
			}
			// ����������K�p
			Vertices[index].Z = FMath::Clamp(Vertices[index].Z, -MaxWaveHight, MaxWaveHight);
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

void AWaterSurface::SetCircleLand(FVector CirclePostion, float Radius)
{
	for (int xi = 1; xi < SplitPointNum.X - 1; ++xi)
	{
		for (int yi = 1; yi < SplitPointNum.Y - 1; ++yi)
		{
			int32 index = CalcIndex(xi, yi);
			float xp = Vertices[index].X;
			float yp = Vertices[index].Y;
			float xc = CirclePostion.X;
			float yc = CirclePostion.Y;
			if ((xp - xc)*(xp - xc) + (yp - yc)*(yp - yc) <= Radius * Radius)
			{
				IsLands[index] = true;
				Vertices[index].Z = CirclePostion.Z;
				UV0[index] = FVector2D((xi / SplitPointNum.X) * 0.5f + 0.5f, (yi / SplitPointNum.Y));
				VertexColors[index] = FLinearColor::Black;
			}
		}
	}
}

void AWaterSurface::SetSquareLand(FVector SquareLocation, float XLength, float YLength)
{
	for (int xi = 1; xi < SplitPointNum.X - 1; ++xi)
	{
		for (int yi = 1; yi < SplitPointNum.Y - 1; ++yi)
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

			IsLands[index] = true;
			Vertices[index].Z = SquareLocation.Z;
			UV0[index] = FVector2D((xi / SplitPointNum.X) * 0.5f + 0.5f, (yi / SplitPointNum.Y));
			VertexColors[index] = FLinearColor::Black;
		}
	}
}

void AWaterSurface::TickFlashFloodWave(AFlashFlood* FlashFlood)
{
	for (int xi = 1; xi < SplitPointNum.X - 1; ++xi)
	{
		for (int yi = 1; yi < SplitPointNum.Y - 1; ++yi)
		{
			int index = CalcIndex(xi, yi);

			if (IsLands[index]) continue;
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

	CreateWave(WaveX, WaveY, power * HeightPower);
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

	if (WaveX >= 0 && WaveX < SplitPointNum.X && WaveY >= 0 && WaveY < SplitPointNum.Y)
	{
		uL = Vertices[CalcIndex(WaveX - 1, WaveY)].Z;
		uR = Vertices[CalcIndex(WaveX + 1, WaveY)].Z;
		uT = Vertices[CalcIndex(WaveX, WaveY - 1)].Z;
		uB = Vertices[CalcIndex(WaveX, WaveY + 1)].Z;
	}
	float HeightValue = 0.0f;
	HeightValue = FMath::Max(HeightValue, FMath::Abs(uL));
	HeightValue = FMath::Max(HeightValue, FMath::Abs(uR));
	HeightValue = FMath::Max(HeightValue, FMath::Abs(uT));
	HeightValue = FMath::Max(HeightValue, FMath::Abs(uB));
	float x = uL - uR;
	float y = uT - uB;

	answerVec += FVector(x, y, 0).GetSafeNormal() * (HeightValue / MaxWaveHight) * MaxWaveHight * WavePower;

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
	if (GetLandPoint(movedPos, circleRadius) != nullptr) 
		return movedPos;

	// �t�B�[���h�̃��b�V���S�̂�����
	for (int xi = 0; xi < SplitPointNum.X; ++xi)
	{
		for (int yi = 0; yi < SplitPointNum.Y; ++yi)
		{
			int index = CalcIndex(xi, yi);
			if (IsLands[index]) continue;	// �n��Ȃ狗���𑪂�K�v���Ȃ��̂Ŏ���

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

	return !IsLands[index];
}

bool AWaterSurface::IsLand(FVector worldPos)
{
	if (!IsInField(worldPos)) return false;

	int32 WaveX = (worldPos.X - Vertices[0].X) / SplitSpace;
	int32 WaveY = (worldPos.Y - Vertices[0].Y) / SplitSpace;

	int index = CalcIndex(WaveX, WaveY);

	if (index <= 1) return false;
	if (index >= Vertices.Num() - 1) return false;

	return IsLands[index];
}

bool AWaterSurface::IsInField(FVector worldPos)
{
	FVector2D Origin = FVector2D(Vertices[0].X, Vertices[0].Y);
	FVector2D End = FVector2D(Vertices[Vertices.Num() - 1].X, Vertices[Vertices.Num() - 1].Y);

	return  Origin.X <= worldPos.X && 
			End.X >= worldPos.X && 
			Origin.Y <= worldPos.Y && 
			End.Y >= worldPos.Y;
}

FVector AWaterSurface::GetGetOffPos(FVector WorldPos, float Radius)
{
	for (int xi = 1; xi < SplitPointNum.X - 1; ++xi)
	{
		for (int yi = 1; yi < SplitPointNum.Y - 1; ++yi)
		{
			if (!IsLands[CalcIndex(xi, yi)]) continue;

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
ALandPoint * AWaterSurface::GetLandPoint(const FVector & WorldPos)
{
	for (auto Actor : LandPointActors)
	{
		if (Actor->OnGround(WorldPos))
		{
			return Actor;
		}
	}
	return nullptr;
}

ALandPoint * AWaterSurface::GetLandPoint(const FVector & WorldPos, float Radius)
{
	for (auto Actor : LandPointActors)
	{
		if (Actor->OnGround(WorldPos, Radius))
		{
			return Actor;
		}
	}
	return nullptr;
}
