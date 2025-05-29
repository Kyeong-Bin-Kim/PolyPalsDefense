// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TowerEnums.h"
#include "TowerStructs.h"
#include "TowerPropertyData.generated.h"

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
	TMap<ELevelValue, FTowerUpgradeValue> UpgradeData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Value|Ability")
	ETowerAbility TowerAbility;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Value|Ability", meta = (EditCondition = "TowerAbility == ETowerAbility::Stun", EditConditionHides))
	TMap<ELevelValue, FStunDetail> StunDetails;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Value|Ability", meta = (EditCondition = "TowerAbility == ETowerAbility::Slow", EditConditionHides))
	TMap<ELevelValue, FSlowDetail> SlowDetails;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Visual")
	TObjectPtr<UStaticMesh> TowerMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Visual")
	TObjectPtr<UStaticMesh> GunMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Visual")
	TObjectPtr<class UNiagaraSystem> MuzzleEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Visual")
	TObjectPtr<class UNiagaraSystem> AoeEffect;
};
