// Fill out your copyright notice in the Description page of Project Settings.


#include "MarchingSquareActor.h"
#include "Kismet/KismetSystemLibrary.h"

//! コンストラクタ
//*****************************************************************************
AMarchingSquareActor::AMarchingSquareActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

//! 開始
//*****************************************************************************
void AMarchingSquareActor::BeginPlay()
{
	Super::BeginPlay();

	FRandomStream rand;
	rand.GenerateNewSeed();

	for (int i = 0; i < Dims; ++i)
	{
		for (int j = 0; j < Dims; ++j)
		{
			PointTable[i][j].Location = FVector(0, Size * i, Size * j);
			PointTable[i][j].OnOff = (bool)rand.RandRange(0, 1);
		}
	}

	for (int i = 0; i < 12; ++i)
	{
		CircleDatas[i].Radius = rand.FRandRange(100.0f, 250.0f);
		CircleDatas[i].Locatoin = FVector(0, rand.FRandRange(0.0f, Size * Dims), rand.FRandRange(0.0f, Size * Dims));
		CircleDatas[i].Movement = FVector(0, rand.FRandRange(-4.0f, 4.0f), rand.FRandRange(-4.0f, 4.0f));
	}
}

//! 更新
//*****************************************************************************
void AMarchingSquareActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ModeCycle += DeltaTime;
	if (ModeCycle >= 3.0f)
	{
		MetaballMode = !MetaballMode;
		ModeCycle = 0.0f;
	}

	if (MetaballMode)
	{
		for (int i = 0; i < 12; ++i)
		{
			CircleDatas[i].Locatoin += CircleDatas[i].Movement;

			if (CircleDatas[i].Locatoin.Y > Size * Dims) CircleDatas[i].Movement.Y *= -1;
			if (CircleDatas[i].Locatoin.Y < 0) CircleDatas[i].Movement.Y *= -1;
			if (CircleDatas[i].Locatoin.Z > Size * Dims) CircleDatas[i].Movement.Z *= -1;
			if (CircleDatas[i].Locatoin.Z < 0) CircleDatas[i].Movement.Z *= -1;
		}
	}
	else
	{
		GeneratePoints();
	}

	for (int i = 0; i < Dims; ++i)
	{
		for (int j = 0; j < Dims; ++j)
		{
			if (MetaballMode)
			{
				PointTable[i][j].OnOff = false;

				for (int k = 0; k < 12; ++k)
				{
					float a = PointTable[i][j].Location.Y - CircleDatas[k].Locatoin.Y;
					float b = PointTable[i][j].Location.Z - CircleDatas[k].Locatoin.Z;
					float c = sqrt(a * a + b * b);
					if (c <= CircleDatas[k].Radius)
					{
						PointTable[i][j].OnOff = true;
					}
				}
			}

			// 頂点をばってんで表示
			float VSize = 0.5f;
			FVector V1 = PointTable[i][j].Location - FVector(0, -VSize, -VSize);
			FVector V2 = PointTable[i][j].Location - FVector(0, VSize, -VSize);
			FVector V3 = PointTable[i][j].Location - FVector(0, VSize, VSize);
			FVector V4 = PointTable[i][j].Location - FVector(0, -VSize, VSize);

			UKismetSystemLibrary::DrawDebugLine(GetWorld(), V1, V3, (PointTable[i][j].OnOff ? FColor::Black : FColor::White));
			UKismetSystemLibrary::DrawDebugLine(GetWorld(), V2, V4, (PointTable[i][j].OnOff ? FColor::Black : FColor::White));

			if ((i >= 1) && (j >= 1)) {
				/*
				* Pointを下記のように定義
				* A---1---B
				* |       |
				* 4       2
				* |       |
				* D---3---C
				*/
				FVector PointA = PointTable[i - 1][j - 1].Location;
				FVector PointB = PointTable[i - 0][j - 1].Location;
				FVector PointC = PointTable[i - 0][j - 0].Location;
				FVector PointD = PointTable[i - 1][j - 0].Location;

				FVector Point1 = PointA + FVector(0, Size * 0.5f, 0);
				FVector Point2 = PointB + FVector(0, 0, Size * 0.5f);
				FVector Point3 = PointC + FVector(0, -Size * 0.5f, 0);
				FVector Point4 = PointD + FVector(0, 0, -Size * 0.5f);

				// 分割ルールに基づいて線を引く
				uint8 BitPattern = 0b0000;
				if (PointTable[i - 1][j - 1].OnOff) BitPattern |= (1 << 3);
				if (PointTable[i - 0][j - 1].OnOff) BitPattern |= (1 << 2);
				if (PointTable[i - 0][j - 0].OnOff) BitPattern |= (1 << 1);
				if (PointTable[i - 1][j - 0].OnOff) BitPattern |= (1 << 0);

				switch (BitPattern)
				{
				case 0b0000:
					break;
				case 0b0001:
					UKismetSystemLibrary::DrawDebugLine(GetWorld(), Point3, Point4, (MetaballMode ? FColor::Green : FColor::Red));
					break;
				case 0b0010:
					UKismetSystemLibrary::DrawDebugLine(GetWorld(), Point2, Point3, (MetaballMode ? FColor::Green : FColor::Red));
					break;
				case 0b0011:
					UKismetSystemLibrary::DrawDebugLine(GetWorld(), Point2, Point4, (MetaballMode ? FColor::Green : FColor::Red));
					break;
				case 0b0100:
					UKismetSystemLibrary::DrawDebugLine(GetWorld(), Point1, Point2, (MetaballMode ? FColor::Green : FColor::Red));
					break;
				case 0b0101:
					UKismetSystemLibrary::DrawDebugLine(GetWorld(), Point1, Point2, (MetaballMode ? FColor::Green : FColor::Red));
					UKismetSystemLibrary::DrawDebugLine(GetWorld(), Point3, Point4, (MetaballMode ? FColor::Green : FColor::Red));
					break;
				case 0b0110:
					UKismetSystemLibrary::DrawDebugLine(GetWorld(), Point1, Point3, (MetaballMode ? FColor::Green : FColor::Red));
					break;
				case 0b0111:
					UKismetSystemLibrary::DrawDebugLine(GetWorld(), Point1, Point4, (MetaballMode ? FColor::Green : FColor::Red));
					break;
				case 0b1000:
					UKismetSystemLibrary::DrawDebugLine(GetWorld(), Point1, Point4, (MetaballMode ? FColor::Green : FColor::Red));
					break;
				case 0b1001:
					UKismetSystemLibrary::DrawDebugLine(GetWorld(), Point1, Point3, (MetaballMode ? FColor::Green : FColor::Red));
					break;
				case 0b1010:
					UKismetSystemLibrary::DrawDebugLine(GetWorld(), Point2, Point3, (MetaballMode ? FColor::Green : FColor::Red));
					UKismetSystemLibrary::DrawDebugLine(GetWorld(), Point1, Point4, (MetaballMode ? FColor::Green : FColor::Red));
					break;
				case 0b1011:
					UKismetSystemLibrary::DrawDebugLine(GetWorld(), Point1, Point2, (MetaballMode ? FColor::Green : FColor::Red));
					break;
				case 0b1100:
					UKismetSystemLibrary::DrawDebugLine(GetWorld(), Point2, Point4, (MetaballMode ? FColor::Green : FColor::Red));
					break;
				case 0b1101:
					UKismetSystemLibrary::DrawDebugLine(GetWorld(), Point2, Point3, (MetaballMode ? FColor::Green : FColor::Red));
					break;
				case 0b1110:
					UKismetSystemLibrary::DrawDebugLine(GetWorld(), Point3, Point4, (MetaballMode ? FColor::Green : FColor::Red));
					break;
				case 0b1111:
					break;
				}
			}
		}
	}
}

//! ポイント生成
//*****************************************************************************
void AMarchingSquareActor::GeneratePoints()
{
	FRandomStream rand;
	rand.GenerateNewSeed();

	float Time = GetWorld()->GetTimeSeconds();

	for (int i = 0; i < Dims; ++i)
	{
		for (int j = 0; j < Dims; ++j)
		{
			PointTable[i][j].Location = FVector(0, Size * i, Size * j);
#if 0
			PointTable[i][j].OnOff = (bool)rand.RandRange(0, 1);
#else
			float Noise = FMath::PerlinNoise3D(FVector(i, j, Time) * 0.1f);
			PointTable[i][j].OnOff = Noise > 0.0f;
#endif
		}
	}
}
