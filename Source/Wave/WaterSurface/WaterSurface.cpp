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

AWaterSurface::AWaterSurface()
{
	PrimaryActorTick.bCanEverTick = true;

	for (int yi = 0; yi < SplitVector.Y; ++yi)
	{
		for (int xi = 0; xi < SplitVector.X; ++xi)
		{
			Vertices.Emplace(0, 0, 0);
			VertexColors.Emplace(WaterColor);
			UV0.Emplace((xi / SplitVector.X) * 0.5f , (yi / SplitVector.Y));
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

	// �g���Z�p���X�g�̏�����
	CurrentHeights.Init(0.0f, SplitVector.X * SplitVector.Y);
	PrevHeights.Init(0.0f, SplitVector.X * SplitVector.Y);
	NewHeights.Init(0.0f, SplitVector.X * SplitVector.Y);

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
	float mul = DeltaSpeed * DeltaSpeed * c * c / (SplitVector.X * SplitVector.Y) * (SplitVector.X * SplitVector.Y);
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

			float damp = Decay * DeltaSpeed * (u - u_pre);

			NewHeights[index] = u + u - u_pre + (mul * (uL + uR + uT + uB - (4.0f * u))) - damp;
		}
	}

	for (int xi = 1; xi < SplitVector.X - 1; ++xi)
	{
		for (int yi = 1; yi < SplitVector.Y - 1; ++yi)
		{
			if (IsLands[CalcIndex(xi, yi)])
			{
				if (abs(NewHeights[CalcIndex(xi, yi)]) > abs(CurrentHeights[CalcIndex(xi, yi)]))
					continue;
			}

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
				Vertices[CalcIndex(xi,yi)].Z = CurrentHeights[CalcIndex(xi,yi)];

			if (Vertices[CalcIndex(xi, yi)].Z > MaxWaveHight)
				Vertices[CalcIndex(xi, yi)].Z = MaxWaveHight;

			VertexColors[CalcIndex(xi, yi)] = FLinearColor::LerpUsingHSV(WaterColor, WaveColor, Vertices[CalcIndex(xi, yi)].Z / MaxWaveHight);
		}
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

	for (int xi = 1; xi < SplitVector.X - 1; ++xi)
	{
		for (int yi = 1; yi < SplitVector.Y - 1; ++yi)
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

FVector2D AWaterSurface::LocationToVertices(FVector Location)
{
	int xi = (Location.X - GetActorLocation().X) / X_Size;
	int yi = (Location.Y - GetActorLocation().Y) / Y_Size;
	return FVector2D(xi, yi);
}

int32 AWaterSurface::CalcIndex(int32 x, int32 y)
{
	int32 index = x + (y * SplitVector.X);
	if (index < 0 ) return 0;
	if (index > SplitVector.X * SplitVector.Y) return 0;
	return index;
}

void AWaterSurface::SetCircleLand(FVector CirclePostion, float Radius)
{
	for (int xi = 1; xi < SplitVector.X - 1; ++xi)
	{
		for (int yi = 1; yi < SplitVector.Y - 1; ++yi)
		{
			float xp = Vertices[CalcIndex(xi, yi)].X;
			float yp = Vertices[CalcIndex(xi, yi)].Y;
			float xc = CirclePostion.X;
			float yc = CirclePostion.Y;
			if ((xp - xc)*(xp - xc) + (yp - yc)*(yp - yc) <= Radius * Radius)
			{
				IsLands[CalcIndex(xi, yi)] = true;
				Vertices[CalcIndex(xi, yi)].Z = CirclePostion.Z;
				UV0[CalcIndex(xi, yi)] = FVector2D((xi / SplitVector.X) * 0.5f + 0.5f, (yi / SplitVector.Y));
			}
		}
	}
}

void AWaterSurface::SetSquareLand(FVector SquareLocation, float XLength, float YLength)
{
	for (int xi = 1; xi < SplitVector.X - 1; ++xi)
	{
		for (int yi = 1; yi < SplitVector.Y - 1; ++yi)
		{
			float xp = Vertices[CalcIndex(xi, yi)].X;
			float yp = Vertices[CalcIndex(xi, yi)].Y;
			float xs = SquareLocation.X;
			float ys = SquareLocation.Y;

			if (xp > xs + XLength * 0.5f) continue;
			if (xp < xs - XLength * 0.5f) continue;
			if (yp > ys + YLength * 0.5f) continue;
			if (yp < ys - YLength * 0.5f) continue;

			IsLands[CalcIndex(xi, yi)] = true;
			Vertices[CalcIndex(xi, yi)].Z = SquareLocation.Z;
			UV0[CalcIndex(xi, yi)] = FVector2D((xi / SplitVector.X) * 0.5f + 0.5f, (yi / SplitVector.Y));
		}
	}
}

void AWaterSurface::AddPower(FVector worldPos, float power = 100.0f)
{
	int32 WaveX = worldPos.X / X_Size;
	int32 WaveY = worldPos.Y / Y_Size;

	//if (WaveX <= 0) return;
	//if (WaveX > SplitVector.X) return;
	//if (WaveY <= 0) return;
	//if (WaveY > SplitVector.Y) return;

	CreateWave(WaveX, WaveY, power);
}

float AWaterSurface::GetWaveHeight(const FVector & worldPos)
{
	if (!IsInWater(worldPos)) return 0.0f;

	int32 WaveX = worldPos.X / X_Size;
	int32 WaveY = worldPos.Y / Y_Size;
	float uL = 0.0f, uR = 0.0f, uT = 0.0f, uB = 0.0f;

	if (WaveX >= 0 && WaveX < SplitVector.X && WaveY >= 0 && WaveY < SplitVector.Y)
	{
		uL = CurrentHeights[CalcIndex(WaveX - 1, WaveY)];
		uR = CurrentHeights[CalcIndex(WaveX + 1, WaveY)];
		uT = CurrentHeights[CalcIndex(WaveX, WaveY - 1)];
		uB = CurrentHeights[CalcIndex(WaveX, WaveY + 1)];
	}

	float result = (uL + uR + uT + uB) * 0.25f;	// �����̕��ς�Ԃ�
	return (result < MaxWaveHight) ? result : MaxWaveHight;	
}

FVector AWaterSurface::GetWavePower(const FVector & worldPos)
{
	if (!IsInWater(worldPos)) return FVector::ZeroVector;

	FVector answerVec = FVector::ZeroVector;

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFlashFlood::StaticClass(), FoundActors);

	for (auto Actor : FoundActors)
	{
		AFlashFlood* FlashFlood = Cast<AFlashFlood>(Actor);
		if (FlashFlood)
		{
			FVector addVec = FlashFlood->GetFloatVec(worldPos);
			addVec.Z = 0;
			answerVec += addVec;
		}
	}

	int32 WaveX = worldPos.X / X_Size;
	int32 WaveY = worldPos.Y / Y_Size;
	float uL = 0.0f, uR = 0.0f, uT = 0.0f, uB = 0.0f;

	if (WaveX >= 0 && WaveX < SplitVector.X && WaveY >= 0 && WaveY < SplitVector.Y)
	{
		uL = CurrentHeights[CalcIndex(WaveX - 1, WaveY)];
		uR = CurrentHeights[CalcIndex(WaveX + 1, WaveY)];
		uT = CurrentHeights[CalcIndex(WaveX, WaveY - 1)];
		uB = CurrentHeights[CalcIndex(WaveX, WaveY + 1)];
	}

	float x = uL - uR;
	float y = uT - uB;

	answerVec += FVector(x, y, 0);

	return answerVec;
}

FVector AWaterSurface::GetOutLandPos(FVector worldPos, float circleRadius)
{
	if (!IsLand(worldPos)) return worldPos;

	for (auto Actor : LandPointActors)
	{
		ACircleLand* CircleLand = Cast<ACircleLand>(Actor);
		if (!CircleLand) continue;

		float distance = FVector::Distance(worldPos, CircleLand->GetActorLocation());
		float judgDistance = circleRadius + CircleLand->GetRadius();
		float landingDistance = judgDistance - distance;
		if (landingDistance <= 0) continue;

		FVector outDirection = worldPos - CircleLand->GetActorLocation();
		outDirection.Z = 0;
		outDirection.Normalize();

		worldPos = worldPos + outDirection * landingDistance;

		if (!IsLand(worldPos)) return worldPos;
	}

	for (auto Actor : LandPointActors)
	{
		ASquareLand* SquareLand = Cast<ASquareLand>(Actor);
		if (!SquareLand) continue;

		FVector judgVec = FVector::ZeroVector;
		FVector moveVec = FVector::ZeroVector;
		OBB2D obb = SquareLand->GetOBB();
		for (int i = 0; i < 2; i++)
		{
			float length = obb.GetLength(i);
			if (length <= 0) continue;
			float dot = FVector::DotProduct(worldPos - obb.GetPosition(), obb.GetDirecton(i)) / length;
			dot = fabs(dot);
			if (dot > 1)
				judgVec += (1 - dot) * obb.GetDirecton(i) * length;

			if ((dot - 1) * length <= circleRadius)
				moveVec += obb.GetDirecton(i) * (((dot - 1) * length) - circleRadius);

		}

		if (judgVec.Size() >= circleRadius) continue;

		moveVec.Y = 0;
		worldPos = worldPos + moveVec;

		if (!IsLand(worldPos)) return worldPos;
	}

	return worldPos;
}

// ���̍��W�𒲂ׂĈړ�����ʒu���Փ˂��Ă����牟���o���֐�
FVector AWaterSurface::AdjustMoveInLand(const FVector & worldPos, const FVector & moveVec, float circleRadius, const FVector & WaterCheckPos, float WaterCheckRadius)
{
	FVector movedPos = worldPos + moveVec;

	// �t�B�[���h�̃��b�V���S�̂�����
	for (int xi = 0; xi < SplitVector.X; ++xi)
	{
		for (int yi = 0; yi < SplitVector.Y; ++yi)
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

				// ���̏�ɑ��݂��Ă��鎞�͈ړ��ł��Ȃ�
				if (!LandActor) return worldPos;

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

		float distance = FVector::Distance(movedPos, FloatActor->GetActorLocation());
		float judgDistance = circleRadius + FloatActor->GetRadius();
		float landingDistance = judgDistance - distance;
		if (landingDistance <= 0) continue;

		FVector outDirection = movedPos - FloatActor->GetActorLocation();
		outDirection.Z = 0;
		outDirection.Normalize();

		// ���݂ɉ����o��
		movedPos += outDirection * landingDistance;

		// ���݂ɗ͂�������
		FVector Power = outDirection * landingDistance;
		moveVec += Power;
		FloatActor->Velocity -= Power;
	}

	// �n�`�Ɠ����蔻��
	for (auto Actor : LandPointActors)
	{
		// �~�`�̒n��Ƃ̓����蔻��
		ACircleLand* CircleLand = Cast<ACircleLand>(Actor);
		if (CircleLand)
		{
			float distance = FVector::Distance(movedPos, CircleLand->GetActorLocation());
			float judgDistance = circleRadius + CircleLand->GetRadius();
			float landingDistance = judgDistance - distance;
			if (landingDistance <= 0) continue;

			FVector outDirection = movedPos - CircleLand->GetActorLocation();
			outDirection.Z = 0;
			outDirection.Normalize();

			movedPos = movedPos + outDirection * landingDistance;

			FVector2D MoveDirection = FVector2D(movedPos - actorPos);
			FVector2D Ref = (MoveDirection - 2.0f * FVector2D::DotProduct(MoveDirection, FVector2D(outDirection)) * FVector2D(outDirection));
			moveVec = FVector(Ref, 0.0f);
			continue;
		}

		// ��`�̒n��Ƃ̓����蔻��
		ASquareLand* SquareLand = Cast<ASquareLand>(Actor);
		if (SquareLand)
		{
			FVector2D SquarePos = FVector2D(SquareLand->GetActorLocation().X, SquareLand->GetActorLocation().Y);
			float XLen = SquareLand->GetXLength() * 0.5f;
			float YLen = SquareLand->GetYLength() * 0.5f;

			float XDistance = (SquarePos.X - movedPos.X);
			float YDistance = (SquarePos.Y - movedPos.Y);
			float JudgXDistance = circleRadius + XLen;
			float JudgYDistance = circleRadius + YLen;

			float X_Deff = JudgXDistance - fabs(XDistance);
			float Y_Deff = JudgYDistance - fabs(YDistance);

			if (X_Deff < 0 || Y_Deff < 0) continue;

			// �Փ˂��Ă���

			FVector2D VecA, VecB, VecC, VecD;
			FVector2D NearPos, NearNormal;
			FRay2D NearSideRay;
			bool IsHit = false;
			float HitDist = 0.0f;
			// ���C�̌�_�𒲂ׂċ������擾����֐�
			auto CheckDistance = [&IsHit, &HitDist, &NearSideRay, &NearNormal, &NearPos, circleRadius]
			(FRay2D RayA, const FRay2D & RayB, const FVector2D & Normal)
			{
				FVector2D HitPos;
				FVector2D CorrectionPos = circleRadius * Normal;
				RayA.Direction -= CorrectionPos;	// �~�̕␳�����炷

				if (MyFunc::ColSegments(RayA, RayB, &HitPos))
				{
					if (IsHit)
					{
						float Dist = FVector2D::Distance(RayA.Origin, HitPos);
						if (Dist < HitDist)
						{
							HitDist = Dist;
							NearSideRay = RayB;
							NearNormal = Normal;
							NearPos = HitPos + CorrectionPos;
						}
					}
					else
					{
						IsHit = true;
						HitDist = FVector2D::Distance(RayA.Origin, HitPos);
						NearSideRay = RayB;
						NearNormal = Normal;
						NearPos = HitPos + CorrectionPos;
					}
				}
			};

			VecA = SquarePos + FVector2D(XLen, YLen);
			VecB = SquarePos + FVector2D(-XLen, YLen);
			VecC = SquarePos + FVector2D(-XLen, -YLen);
			VecD = SquarePos + FVector2D(XLen, -YLen);

			FRay2D PosToMovedPos = FRay2D(actorPos, movedPos - actorPos);
			CheckDistance(PosToMovedPos, FRay2D(VecA, VecB - VecA), FVector2D(0.0f, 1.0f));
			CheckDistance(PosToMovedPos, FRay2D(VecB, VecC - VecB), FVector2D(-1.0f, 0.0f));
			CheckDistance(PosToMovedPos, FRay2D(VecC, VecD - VecC), FVector2D(0.0f, -1.0f));
			CheckDistance(PosToMovedPos, FRay2D(VecD, VecA - VecD), FVector2D(1.0f, 0.0f));

			// �ӂɏՓ˂��Ă�����
			if (IsHit)
			{
				// �Փ˓_�Ɉʒu���C��
				movedPos = FVector(NearPos, movedPos.Z);
				// ���˃x�N�g�����Z�o
				FVector2D Ref = (PosToMovedPos.Direction - 2.0f * FVector2D::DotProduct(PosToMovedPos.Direction, NearNormal) * NearNormal);
				moveVec = FVector(Ref, 0.0f);
			}
			// ���ɖ��܂��Ă�����
			else
			{
				// X���̉����o���i�Ȃ��Ƃ͎v�����ǈꉞ���܂�h�~�p�j
				movedPos.X += (XDistance > 0) ? -X_Deff : X_Deff;
			}
		}
	}
	
	return movedPos;
}

bool AWaterSurface::IsInWater(FVector worldPos)
{
	int32 WaveX = worldPos.X / X_Size;
	int32 WaveY = worldPos.Y / Y_Size;

	int index = CalcIndex(WaveX, WaveY);

	if (index <= 1) return false;
	if (index >= Vertices.Num() - 1) return false;

	return true;
}

bool AWaterSurface::IsLand(FVector worldPos)
{
	if (!IsInWater(worldPos)) return false;

	int32 WaveX = worldPos.X / X_Size;
	int32 WaveY = worldPos.Y / Y_Size;

	int index = CalcIndex(WaveX, WaveY);

	return IsLands[index];
}

FVector AWaterSurface::GetGetOffPos(FVector WorldPos, float Radius)
{
	for (int xi = 1; xi < SplitVector.X - 1; ++xi)
	{
		for (int yi = 1; yi < SplitVector.Y - 1; ++yi)
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
