// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataAsset/Tower/TowerEnums.h"
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
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void ServerOnEnemyBeginOverlap(AActor* InEnemy);
	void ServerOnEnemyEndOverlap(AActor* InEnemy);
	AActor* ServerFindFirstValidTarget();
	void ServerOnTowerAttack();
	void ClientOnTowerAttack();
	void ClientUpdateGunMeshRotation();
	void ServerSetTowerIdByTower(uint8 InTowerId);
	void SetAttackTimer();
	void ClearAttackTimer();

	UFUNCTION()
	void OnRep_TowerId();

	UFUNCTION()
	void OnRep_CurrentTarget();

	void ServerOnTowerLevelUp();
	UFUNCTION()
	void OnRep_CurrentLevel();

private:
	UPROPERTY(ReplicatedUsing = OnRep_CurrentLevel)
	uint8 CurrentLevel = 1;

	UPROPERTY(ReplicatedUsing = OnRep_TowerId);
	int16 TowerId = -1;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentTarget)
	TObjectPtr<AActor> CurrentTarget;
	
	UPROPERTY()
	TArray<AActor*> SpottedEnemy_Server;


	float Damage = 0.f;
	float AttackDelay = -1.f;
	ETowerAbility TowerAbility = ETowerAbility::None;

	FTimerHandle AttackHandle;
	FTimerHandle GunMeshHandle;
	FTimerHandle DelayHandle;

private:
	UPROPERTY()
	TObjectPtr<class APlacedTower> OwnerTower;
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> GunMeshComponent;
	UPROPERTY()
	TObjectPtr<class UNiagaraSystem> MuzzleEffect;

	friend APlacedTower;

};
