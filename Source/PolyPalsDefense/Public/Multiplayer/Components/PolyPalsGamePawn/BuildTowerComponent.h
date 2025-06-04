// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataAsset/Tower/TowerEnums.h"
#include "BuildTowerComponent.generated.h"

UENUM()
enum class EBuildState
{
	None = 0,
	SerchingPlace,
	DecidePlacementLocation
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POLYPALSDEFENSE_API UBuildTowerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBuildTowerComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// Call by gamepawn
	void ClientSpawnPreviewBuilding();

private:
	void SetPlayerColorByController(EPlayerColor InColor);

	void ClientOnInputTest();
	void ClientOnInputClick();
	void ClientOnInputRightClick();
	void ClientOnInputTower1();
	void ClientOnInputTower2();
	void ClientOnInputTower3();

	void OnSelectTower(const uint8 InTowerId);

	void SetBuildState(EBuildState InState);
	void OnNormal();
	void OnSerchingPlace();
	void OnDecidePlacementLocation();

	UFUNCTION(Server, Reliable)
	void Server_RequestSpawnTower(const FVector_NetQuantize InLocation, uint8 InTargetTower);

private:
	UPROPERTY()
	TObjectPtr<class UTowerDataManager> TowerDataManager;
	UPROPERTY()
	TObjectPtr<class APreviewBuilding> PreviewBuilding;
	UPROPERTY()
	TSubclassOf<class APlacedTower> PlacedTowerBlueClass;

	UPROPERTY(Replicated)
	EPlayerColor PlayerColor = EPlayerColor::None;

	uint8 TowerOnSerchingQue = 0;
	EBuildState BuildState = EBuildState::None;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UTowerUpgradeWidget> TowerUpgradeWidgetClass;

	UPROPERTY()
	TObjectPtr<UTowerUpgradeWidget> UpgradeWidgetInstance;

	friend class APolyPalsGamePawn;
	friend class APolyPalsController;
};
