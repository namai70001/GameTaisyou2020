// �g�̉��Z�ɂ͔g�����������g�p���Ă���
// �ȉ��g���������̎Q�l�T�C�g
// https://qiita.com/Ushio/items/0249fd7a5363ccd914dd
// https://www.slideshare.net/UnityTechnologiesJapan/unite-2017-tokyo3d-76689196
// https://www.nicovideo.jp/watch/sm9470923

#include "WaterSurface.h"

AWaterSurface::AWaterSurface()
{
	PrimaryActorTick.bCanEverTick = true;

	for (int yi = 0; yi < SplitVector.Y; ++yi)
	{
		for (int xi = 0; xi < SplitVector.X; ++xi)
		{
			Vertices.Emplace(0, 0, 0);
		}
	}

	IsLands.Init(false, Vertices.Num());

	for (int yi = 0; yi < SplitVector.Y - 1; yi++)
	{
		for (int xi = 0; xi < SplitVector.X - 1; xi++)
		{
			Triangles.Emplace((SplitVector.Y * (yi + 0)) + xi);
			Triangles.Emplace((SplitVector.Y * (yi + 1)) + xi);
			Triangles.Emplace((SplitVector.Y * (yi + 0)) + xi + 1);
			Triangles.Emplace((SplitVector.Y * (yi + 0)) + xi + 1);
			Triangles.Emplace((SplitVector.Y * (yi + 1)) + xi);
			Triangles.Emplace((SplitVector.Y * (yi + 1)) + xi + 1);
		}
	}
	CreateMesh();
}

void AWaterSurface::BeginPlay()
{
	Super::BeginPlay();

	X_Size = (-StartPoint->GetActorLocation().X + EndPoint->GetActorLocation().X) / SplitVector.X;
	Y_Size = (-StartPoint->GetActorLocation().Y + EndPoint->GetActorLocation().Y) / SplitVector.Y;

	for (int yi = 0; yi < SplitVector.Y; ++yi)
	{
		for (int xi = 0; xi < SplitVector.X; ++xi)
		{
			Vertices[CalcIndex(xi, yi)].X = X_Size * xi;
			Vertices[CalcIndex(xi, yi)].Y = Y_Size * yi;
		}
	}

	UpdateMesh();

	// �g���Z�p���X�g�̏�����
	CurrentHeights.Init(0.0f, SplitVector.X * SplitVector.Y);
	PrevHeights.Init(0.0f, SplitVector.X * SplitVector.Y);
	NewHeights.Init(0.0f, SplitVector.X * SplitVector.Y);

	int cx1 = SplitVector.X / 3;
	int cy1 = SplitVector.Y / 3;

	SetLand(0, 0, SplitVector.X, cy1);
	SetLand(cx1 * 2, cy1, SplitVector.X, cy1 * 2);
	SetLand(cx1, cy1 * 2, SplitVector.X, SplitVector.Y);
}

void AWaterSurface::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float c = (DeltaTime * 10.0f) * (DeltaTime * 10.0f);
	for (int xi = 1; xi < SplitVector.X - 1; ++xi)
	{
		for (int yi = 1; yi < SplitVector.Y - 1; ++yi)
		{
			int32 index = CalcIndex(xi, yi);

			float uL = CurrentHeights[this->CalcIndex(xi - 1, yi)];
			float uR = CurrentHeights[this->CalcIndex(xi + 1, yi)];
			float uT = CurrentHeights[this->CalcIndex(xi, yi - 1)];
			float uB = CurrentHeights[this->CalcIndex(xi, yi + 1)];

			float u_pre = PrevHeights[index];
			float u = CurrentHeights[index];

			float k = 0.1f;
			float damp = -k * DeltaTime * (u - u_pre);

			NewHeights[index] = (2.0f * u) + (c * (uL + uR + uT + uB - (4.0f * u))) - u_pre + damp;
		}
	}

	for (int xi = 1; xi < SplitVector.X - 1; ++xi)
	{
		for (int yi = 1; yi < SplitVector.Y - 1; ++yi)
		{
			int32 index = CalcIndex(xi, yi);
			PrevHeights[index] = CurrentHeights[index];
			CurrentHeights[index] = NewHeights[index];
		}
	}

	for (int yi = 0; yi < SplitVector.X; ++yi)
	{
		for (int xi = 0; xi < SplitVector.Y; ++xi)
		{
			if(!IsLands[CalcIndex(xi,yi)])
				Vertices[CalcIndex(xi,yi)].Z += CurrentHeights[CalcIndex(xi,yi)];
		}
	}

	UpdateMesh();
}

void AWaterSurface::CreateWave(int32 x, int32 y)
{
	// �K�E�X���Z
	auto gauss = [](float x, float sigma)
	{
		return 1.0f / sqrt(PI * 2) * sigma * exp(-x * x / (2.0f * sigma * sigma));
	};

	FVector2D wv = FVector2D(x, y);

	for (int xi = 1; xi < SplitVector.X - 1; ++xi)
	{
		for (int yi = 1; yi < SplitVector.Y - 1; ++yi)
		{
			int index = CalcIndex(xi, yi);
			float value = 0.0f;

			FVector2D v = FVector2D(xi, yi);

			float norm = FVector2D::Distance(v, wv);
			value += gauss(norm, 1.0f) * 100.0f;

			CurrentHeights[index] += value;
			PrevHeights[index] += value;
			NewHeights[index] += value;
		}
	}
}

int32 AWaterSurface::CalcIndex(int32 x, int32 y)
{
	return x + (y * SplitVector.X);
}

void AWaterSurface::SetLand(int32 sx, int32 sy, int32 ex, int32 ey)
{
	for (int xi = sx; xi < ex; ++xi)
	{
		for (int yi = sy; yi < ey; ++yi)
		{
			IsLands[CalcIndex(xi, yi)] = true;
			Vertices[CalcIndex(xi, yi)].Z = 10.0f;
		}
	}
}

void AWaterSurface::AddPawer(FVector worldPos)
{
	int32 WaveX = worldPos.X / X_Size;
	int32 WaveY = worldPos.Y / Y_Size;

	CreateWave(WaveX, WaveY);
}