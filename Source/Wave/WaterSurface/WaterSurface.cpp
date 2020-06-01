// 波の演算には波動方程式を使用している
// 以下波動方程式の参考サイト
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

	// 頂点の数をフィールドの大きさに応じて決定する
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

	// メンバの頂点データなどからメッシュを生成
	CreateMesh();

	// 波演算用リストの初期化
	CurrentHeights.Init(0.0f, SplitPointNum.X * SplitPointNum.Y);
	PrevHeights.Init(0.0f, SplitPointNum.X * SplitPointNum.Y);
	NewHeights.Init(0.0f, SplitPointNum.X * SplitPointNum.Y);

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALandPoint::StaticClass(), FoundActors);
	
	for (auto Actor : FoundActors)
	{
		// 結構処理に使うのでメンバに登録しておく
		LandPointActors.Add(Cast<ALandPoint>(Actor));

		// 円形の地形の初期化
		ACircleLand* CircleLand = Cast<ACircleLand>(Actor);
		if (CircleLand)
		{
			SetCircleLand(CircleLand->GetActorLocation(), CircleLand->GetRadius());
			continue;
		}
		// 矩形の地形の初期化
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
		// 結構処理に使うのでメンバに登録しておく
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
				// 高さに応じてカラーを変更
				VertexColors[index] = FLinearColor::LerpUsingHSV(WaterColor, WaveColor, FMath::Abs(Vertices[index].Z) / MaxWaveHight);
			}
			// 高さ制限を適用
			Vertices[index].Z = FMath::Clamp(Vertices[index].Z, -MaxWaveHight, MaxWaveHight);
		}
	}

	// 普通の波にならないよう水流は最後に計算
	for (auto Actor : FlashFloods)
	{
		TickFlashFloodWave(Actor);
	}

	UpdateMesh();
}

void AWaterSurface::CreateWave(int32 x, int32 y, float power)
{
	// ガウス演算
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

			// 波と水流両方の影響がある場合、高さの高い方を優先
			//Vertices[index].Z = Vertices[index].Z < NewHight ? NewHight : Vertices[index].Z;

			// 加算した方が面白いのでこっち！
			Vertices[index].Z = Vertices[index].Z + NewHight;

			// 高さ制限を適用
			Vertices[index].Z = FMath::Clamp(Vertices[index].Z, -MaxWaveHight, MaxWaveHight);

			// 高さに応じてカラーを変更
			VertexColors[index] = FLinearColor::LerpUsingHSV(WaterColor, WaveColor, FMath::Abs(Vertices[index].Z) / MaxWaveHight);
		}
	}
}

void AWaterSurface::AddPower(FVector worldPos, float power = 100.0f)
{
	int32 WaveX = (worldPos.X - Vertices[0].X) / SplitSpace;
	int32 WaveY = (worldPos.Y - Vertices[0].Y) / SplitSpace;
	
	CreateWave(WaveX, WaveY, power);
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

	float result = (uL + uR + uT + uB) * 0.25f;	// 高さの平均を返す
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

	float x = uL - uR;
	float y = uT - uB;

	answerVec += FVector(x, y, 0);

	return answerVec;
}

FVector AWaterSurface::AdjustMoveInField(const FVector & worldPos, float circleRadius)
{
	FVector Result = worldPos;

	float Right_X = EndPoint->GetActorLocation().X;
	float Left_X = StartPoint->GetActorLocation().X;
	float Up_Y = EndPoint->GetActorLocation().Y;
	float Bottom_Y = StartPoint->GetActorLocation().Y;

	// X軸を確認
	float Deff = Result.X + circleRadius - (Right_X);
	if (Deff > 0.0f) Result.X -= Deff;
	else
	{
		Deff = Result.X - circleRadius - (Left_X);
		if (Deff < 0.0f) Result.X -= Deff;
	}
	// Y軸を確認
	Deff = Result.Y + circleRadius - (Up_Y);
	if (Deff > 0.0f) Result.Y -= Deff;
	else
	{
		Deff = Result.Y - circleRadius - (Bottom_Y);
		if (Deff < 0.0f) Result.Y -= Deff;
	}

	return Result;
}

// 水の座標を調べて移動する位置が衝突していたら押し出す関数
FVector AWaterSurface::AdjustMoveInLand(const FVector & worldPos, const FVector & moveVec, float circleRadius, const FVector & WaterCheckPos, float WaterCheckRadius)
{
	FVector movedPos = worldPos + moveVec;

	// フィールドの外に出ないようにする
	movedPos = AdjustMoveInField(movedPos, circleRadius);

	// フィールドのメッシュ全体を検索
	for (int xi = 0; xi < SplitPointNum.X; ++xi)
	{
		for (int yi = 0; yi < SplitPointNum.Y; ++yi)
		{
			int index = CalcIndex(xi, yi);
			if (IsLands[index]) continue;	// 地上なら距離を測る必要がないので次へ

			float xp = Vertices[index].X;
			float yp = Vertices[index].Y;
			float xc = WaterCheckPos.X;
			float yc = WaterCheckPos.Y;

			// 移動先の座標と水との距離を測る
			if ((xp - xc)*(xp - xc) + (yp - yc)*(yp - yc) <= WaterCheckRadius * WaterCheckRadius)
			{	
				// ここで移動先が水と分かったので今いる地面を調べる
				// 地形の形によってそれぞれ処理を書く
				ALandPoint * LandActor = GetLandPoint(worldPos);

				// 水の上に存在している時は移動できない
				if (!LandActor) return worldPos;

				return LandActor->AdjustMoveInLand(movedPos, circleRadius);
			}
		}
	}

	// 移動先が地上だったのでそのまま移動
	return movedPos;
}


FVector AWaterSurface::AdjustMoveInWater(const AActor * Object, FVector& moveVec, float circleRadius)
{
	FVector actorPos = Object->GetActorLocation();
	FVector movedPos = actorPos + moveVec;

	TArray<AActor*> FoundActors;
	// 他のFloatActorとの当たり判定
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

		// 相互に押し出す
		movedPos += outDirection * landingDistance;

		// 相互に力を加える
		FVector Power = outDirection * landingDistance;
		moveVec += Power;
		FloatActor->Velocity -= Power;
	}

	// 地形と当たり判定
	for (auto Actor : LandPointActors)
	{
		// 円形の地上との当たり判定
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

		// 矩形の地上との当たり判定
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

			// 衝突している

			FVector2D VecA, VecB, VecC, VecD;
			FVector2D NearPos, NearNormal;
			FRay2D NearSideRay;
			bool IsHit = false;
			float HitDist = 0.0f;
			// レイの交点を調べて距離を取得する関数
			auto CheckDistance = [&IsHit, &HitDist, &NearSideRay, &NearNormal, &NearPos, circleRadius]
			(FRay2D RayA, const FRay2D & RayB, const FVector2D & Normal)
			{
				FVector2D HitPos;
				FVector2D CorrectionPos = circleRadius * Normal;
				RayA.Direction -= CorrectionPos;	// 円の補正分ずらす

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

			// 辺に衝突していた時
			if (IsHit)
			{
				// 衝突点に位置を修正
				movedPos = FVector(NearPos, movedPos.Z);
				// 反射ベクトルを算出
				FVector2D Ref = (PosToMovedPos.Direction - 2.0f * FVector2D::DotProduct(PosToMovedPos.Direction, NearNormal) * NearNormal);
				moveVec = FVector(Ref, 0.0f);
			}
			// 既に埋まっている状態
			else
			{
				// X軸の押し出し（ないとは思うけど一応埋まり防止用）
				movedPos.X += (XDistance > 0) ? -X_Deff : X_Deff;
			}
		}
	}
	
	return movedPos;
}

bool AWaterSurface::IsInWater(FVector worldPos)
{
	int32 WaveX = (worldPos.X - Vertices[0].X) / SplitSpace;
	int32 WaveY = (worldPos.Y - Vertices[0].Y) / SplitSpace;

	int index = CalcIndex(WaveX, WaveY);

	if (index <= 1) return false;
	if (index >= Vertices.Num() - 1) return false;

	return true;
}

bool AWaterSurface::IsLand(FVector worldPos)
{
	if (!IsInWater(worldPos)) return false;

	int32 WaveX = (worldPos.X - Vertices[0].X) / SplitSpace;
	int32 WaveY = (worldPos.Y - Vertices[0].Y) / SplitSpace;

	int index = CalcIndex(WaveX, WaveY);

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

// 引数の座標の位置にある地面を取得
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
