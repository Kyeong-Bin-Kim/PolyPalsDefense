// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TowerStructs.generated.h"

USTRUCT(BlueprintType)
struct FStunDetail
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (ClampMin = "0"))
	float Duration = 0.f;
};

USTRUCT(BlueprintType)
struct FSlowDetail
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (ClampMin = "0.0"))
	float Duration = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0", UIMax = "1"))
	float Intensity = 0.f;
};

USTRUCT(BlueprintType)
struct FTowerUpgradeValue
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Damage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float AttackDelay;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Range;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (ClampMin = "0"))
	int32 Cost;

	FTowerUpgradeValue() : Damage(1.f), AttackDelay(1.f), Range(1.f), Cost(0) {}
};