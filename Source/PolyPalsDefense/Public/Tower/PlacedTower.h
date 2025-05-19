// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataAsset/Tower/TowerEnums.h"
#include "PlacedTower.generated.h"

UCLASS()
class POLYPALSDEFENSE_API APlacedTower : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlacedTower();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void ExternalInitializeTower(uint8 InTowerId, EPlayerColor InColor);
	UFUNCTION()
	void OnRep_PlayerColor();
	UFUNCTION()
	void OnRep_TowerId();

	void ClientSetTowerMeshComponent(uint8 InTowerId, EPlayerColor InColor);

private:
	void SetTowerCollision();

	

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UBoxComponent> RootBoxComponent;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> TowerMeshComponent;

private: 
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> GunMeshComponent;
	UPROPERTY(ReplicatedUsing = OnRep_PlayerColor)
	EPlayerColor PlayerColor = EPlayerColor::None;
	UPROPERTY(ReplicatedUsing = OnRep_TowerId)
	int16 TowerId = -1;

	FTimerHandle VisualSetupHandle;
};
