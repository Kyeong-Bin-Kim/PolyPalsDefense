// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataAsset/Tower/TowerEnums.h"
#include "TowerHandleComponent.generated.h"

class APolyPalsGamePawn;
class APolyPalsController;
class UBuildTowerComponent;
class APlacedTower;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POLYPALSDEFENSE_API UTowerHandleComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTowerHandleComponent();

protected:
	virtual void BeginPlay() override;

public:	
	//virtual void InitializeComponent() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UFUNCTION()
	void ClientOnInputClick();

	void SetBuildState(EBuildState InState) { BuildState = InState; }

private:
	UPROPERTY(Replicated)
	TObjectPtr<APolyPalsController> PolyPalsController;
	UPROPERTY()
	TObjectPtr<APlacedTower> FocusedTower;

	EBuildState BuildState;
	
	friend APolyPalsGamePawn;
	friend UBuildTowerComponent;
};
