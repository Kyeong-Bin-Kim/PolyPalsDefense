// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
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
