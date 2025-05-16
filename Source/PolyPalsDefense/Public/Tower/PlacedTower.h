// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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

private: // Components
	UPROPERTY()
	TObjectPtr<class USceneComponent> RootSceneComponent;
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> TowerMeshComponent;
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> GunMeshComponent;

};
