// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataAsset/Tower/TowerEnums.h"
#include "UI/TowerUpgradeWidget.h"
#include "BuildTowerComponent.generated.h"

class UTowerDataManager;
class UTowerHandleComponent;
class APreviewBuilding;
class APlacedTower;
class APolyPalsGamePawn;
class APolyPalsController;


DECLARE_DELEGATE(FTryBuildButNotEnoughGold)

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class POLYPALSDEFENSE_API UBuildTowerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBuildTowerComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void InitializeComponent() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// Call by gamepawn
	void ClientSpawnPreviewBuilding();

private:
	void SetPlayerColorByController(EPlayerColor InColor);

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

public:
	FTryBuildButNotEnoughGold TryBuildButNotEnoughGold;

private:
	UPROPERTY()
	TObjectPtr<UTowerDataManager> TowerDataManager;
	UPROPERTY()
	TObjectPtr<APreviewBuilding> PreviewBuilding;
	UPROPERTY()
	TSubclassOf<APlacedTower> PlacedTowerBlueClass;
	UPROPERTY()
	TObjectPtr<APolyPalsGamePawn> GamePawn;
	UPROPERTY()
	TObjectPtr<UTowerHandleComponent> TowerHandleComponent;

	UPROPERTY(Replicated)
	EPlayerColor PlayerColor = EPlayerColor::None;

	uint8 TowerOnSerchingQue = 0;
	EBuildState BuildState = EBuildState::None;

	//<<<<<<< HEAD
		friend APolyPalsGamePawn;
	friend APolyPalsController;
	//=======
		UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UTowerUpgradeWidget> TowerUpgradeWidgetClass;

	UPROPERTY()
	TObjectPtr<UTowerUpgradeWidget> UpgradeWidgetInstance;
	friend class APolyPalsGamePawn;
	friend class APolyPalsController;

	//>>>>>>> UI
};
