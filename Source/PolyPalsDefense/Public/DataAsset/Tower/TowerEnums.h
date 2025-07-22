// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM()
enum class EPlayerColor : uint8
{
	None = 0,
	Red,
	Blue,
	Yellow,
	Green,
	MAXCOLOR
};
UENUM()
enum class ETowerAbility : uint8
{
	None = 0,
	Stun,
	Slow
};
UENUM()
enum class EAttackMethod : uint8
{
	Projectile,
	Delegate,
	LineTrace
};
UENUM()
enum class ELevelValue : uint8
{
	Level1			UMETA(DisplayName = "1??덇볼"),
	Level2			UMETA(DisplayName = "2??덇볼"),
	Level3			UMETA(DisplayName = "3??덇볼"),
	MaxLevel		UMETA(Hidden)
};
UENUM()
enum class EBuildState : uint8
{
	None = 0,
	SearchingPlace,
	DecidePlacementLocation
};