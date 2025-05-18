// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TowerPropertyData.generated.h"

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
	Level1 = 0		UMETA(DisplayName = "1레벨"),
	Level2			UMETA(DisplayName = "2레벨"),
	Level3			UMETA(DisplayName = "3레벨")
};

USTRUCT(BlueprintType)
struct FTowerUpgradeValue
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Damage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float AttackDelay;
};

UCLASS()
class POLYPALSDEFENSE_API UTowerPropertyData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("TowerData", GetFName());
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Value")
	FText TowerName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Value")
	uint8 TowerId;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Value")
	ETowerAbility TowerAbility;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Value")
	TMap<ELevelValue, FTowerUpgradeValue> UpgradeData;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Value")
	EAttackMethod AttackMethod;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Visual")
	TObjectPtr<UStaticMesh> TowerMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Visual")
	TObjectPtr<UStaticMesh> GunMesh;
};
