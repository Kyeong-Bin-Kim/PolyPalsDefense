// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TowerEnums.h"
#include "TowerPropertyData.generated.h"

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
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite,meta = (ClampMin = "0"))
	int32 Cost;
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Visual")
	TObjectPtr<class UNiagaraSystem> MuzzleEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Visual")
	TObjectPtr<class UNiagaraSystem> AoeEffect;
};
