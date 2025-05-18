// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
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
	// Sets default values for this component's properties
	UBuildTowerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Call by gamepawn
	void ClientSpawnPreviewBuilding();
private:
	void ClientOnInputTest();
	void ClientOnInputClick();

	void OnSelectTower(const uint8 InTowerId);

	void SetBuildState(EBuildState InState);
	void OnNormal();
	void OnSerchingPlace();
	void OnDecidePlacementLocation();
private:
	UPROPERTY()
	TObjectPtr<class UTowerDataManager> TowerDataManager;
	UPROPERTY()
	TObjectPtr<class APreviewBuilding> PreviewBuilding;

	EBuildState BuildState = EBuildState::None;
		
	friend class APolyPalsGamePawn;
};
