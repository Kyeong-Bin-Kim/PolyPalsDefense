// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DataAsset/Tower/TowerEnums.h"
#include "DataAsset/Tower/TowerStructs.h"
#include "TowerDataManager.generated.h"


class UTowerMaterialData;
class UTowerPropertyData;
UCLASS()
class POLYPALSDEFENSE_API UTowerDataManager : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	UTowerMaterialData* GetTowerMaterialData() const { return TowerMaterials; }
	UTowerPropertyData* GetTowerPropertyData(const uint8 InTowerId);
	int32 GetTowerCost(uint8 InTowerId, ELevelValue InLevel);
	float GetAbilityDuration(uint8 InTowerId, uint8 InLevel);
	float GetAbilityIntensity(uint8 InTowerId, uint8 InLevel);

private:
	void LoadMaterialData();
	void LoadPropertyData();
	void OnTowerMaterialLoaded(FPrimaryAssetId LoadedId);
	void OnTowerPropertyLoaded(FPrimaryAssetId LoadedId);

private:
	UPROPERTY()
	TObjectPtr<UTowerMaterialData> TowerMaterials;
	UPROPERTY()
	TMap<uint8, UTowerPropertyData*> TowerProperties;

};
