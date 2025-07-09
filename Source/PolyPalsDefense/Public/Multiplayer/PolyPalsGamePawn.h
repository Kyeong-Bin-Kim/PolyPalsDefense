// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PolyPalsGamePawn.generated.h"

class APolyPalsController;
class USceneComponent;
class USpringArmComponent;
class UCameraComponent;
class UBuildTowerComponent;
class UTowerHandleComponent;

UCLASS()
class POLYPALSDEFENSE_API APolyPalsGamePawn : public APawn
{
	GENERATED_BODY()

public:
	APolyPalsGamePawn();

protected:
	virtual void BeginPlay() override;

public:
	FVector GetSpectateLocation() const { return SpectateLocation; }
	UBuildTowerComponent* GetBuildTowerComponent() const { return BuildTowerComponent; }
	UTowerHandleComponent* GetTowerHandleComponent() const { return TowerHandleComponent; }

protected:
	UPROPERTY(EditDefaultsOnly)
	FVector SpectateLocation;

private:
	// Components
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USceneComponent> RootSceneComponent;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCameraComponent> PolyPalsPlayCamera;
	
private:
	// ActorComponents
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TObjectPtr<UBuildTowerComponent> BuildTowerComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TObjectPtr<UTowerHandleComponent> TowerHandleComponent;

	friend class UBuildTowerComponent;

};
