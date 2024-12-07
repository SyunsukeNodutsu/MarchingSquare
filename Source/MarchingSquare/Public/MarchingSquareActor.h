// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MarchingSquareActor.generated.h"

/**
 * マーチングスクエア
 */
UCLASS()
class MARCHINGSQUARE_API AMarchingSquareActor : public AActor
{
	GENERATED_BODY()
	
public:
	// 点群の生成
	UFUNCTION(BlueprintCallable)
	void GeneratePoints();

public:
	AMarchingSquareActor();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:

	// メタボールモードかどうか
	bool MetaballMode = false;

	float ModeCycle = 0.0f;

	// 辺の長さ
	static const int Size = 12;

	// 分割数
	static const int Dims = 128;

	// 頂点データ
	struct PointData
	{
		FVector Location = FVector(0);
		bool OnOff = false;
	};
	PointData PointTable[Dims][Dims];

	// メタボールのサークルデータ
	struct CircleData
	{
		FVector Locatoin = FVector(0);
		FVector Movement = FVector(0);
		float Radius = 0.0f;
	};
	CircleData CircleDatas[12];

};
