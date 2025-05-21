// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TowerAttackComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POLYPALSDEFENSE_API UTowerAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTowerAttackComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void InitializeComponent() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void ServerSetTowerIdByTower(uint8 InTowerId);

	void ServerOnTowerLevelUp();
	UFUNCTION()
	void OnRep_CurrentLevel();

private:
	UPROPERTY(ReplicatedUsing = OnRep_CurrentLevel)
	uint8 CurrentLevel = 1;

	UPROPERTY(Replicated);
	uint8 TowerId = -1;

	float Damage = 0.f;
	float AttackDelay = 0.f;

private:
	UPROPERTY()
	TObjectPtr<class APlacedTower> OwnerTower;
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> GunMeshComponent;

	friend APlacedTower;

};
