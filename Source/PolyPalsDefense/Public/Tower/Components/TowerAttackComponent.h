#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataAsset/Tower/TowerEnums.h"
#include "DataAsset/Tower/TowerStructs.h"
#include "TowerAttackComponent.generated.h"

class AEnemyPawn;

UENUM()
enum class ETowerState : uint8
{
	Idle = 0,
	SpotTarget,
	Attack,
	Delay,
	LostTarget
};

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
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void ServerOnEnemyBeginOverlap(AActor* InEnemy);
	void ServerOnEnemyEndOverlap(AActor* InEnemy);
	void ClientUpdateGunMeshRotation();
	void ServerSetTowerIdByTower(uint8 InTowerId);
	void SetGunMeshTimer();
	void ClearTowerTimer(FTimerHandle& InHandle);

	void HandleNewTarget();
	void SetTowerState(ETowerState InState);
	void OnIdle();
	void OnSpotTarget();
	void OnAttack();
	void OnDelay();
	void OnLostTarget();

	UFUNCTION()
	void OnRep_TowerId();

	UFUNCTION(Server, Reliable)
	void ServerSetCurrentTarget(AEnemyPawn* NewTarget);

	UFUNCTION()
	void OnRep_CurrentTarget();

	UFUNCTION(Server, Reliable)
	void Server_OnTowerLevelUp();

	UFUNCTION()
	void OnRep_CurrentLevel();

	UFUNCTION()
	void OnRep_MuzzleEffect();

	UFUNCTION()
	void OnRep_bAoeEffect();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayMuzzleEffect();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayAoeEffect(FVector_NetQuantize InLocation);

	UFUNCTION()
	void ClientOnClickedUpgrade();

private:
	UPROPERTY(ReplicatedUsing = OnRep_CurrentLevel)
	uint8 CurrentLevel = 1;

	UPROPERTY(ReplicatedUsing = OnRep_TowerId);
	int16 TowerId = -1;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentTarget)
	TObjectPtr<class AEnemyPawn> CurrentTarget;

	UPROPERTY(ReplicatedUsing = OnRep_MuzzleEffect)
	bool bMuzzleEffect = false;

	UPROPERTY(ReplicatedUsing = OnRep_bAoeEffect)
	bool bAoeEffect = false;

	UPROPERTY(Replicated)
	FVector_NetQuantize AoeLocation;

	ETowerState TowerState_Server = ETowerState::Idle;
	
	UPROPERTY()
	TArray<AEnemyPawn*> SpottedEnemy_Server;

	bool bReadyToAttack = true;
	float Damage = 0.f;
	float AttackDelay = -1.f;
	ETowerAbility TowerAbility = ETowerAbility::None;
	float AbilityDuration = 0.f;
	float AbilityIntensity = 0.f;

	FTimerHandle AttackHandle;
	FTimerHandle GunMeshHandle;
	FTimerHandle DelayHandle;

private:
	UPROPERTY()
	TObjectPtr<class APlacedTower> OwnerTower;

	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> GunMeshComponent;

	UPROPERTY()
	TObjectPtr<class UNiagaraComponent> MuzzleEffectComponent;

	UPROPERTY(Replicated)
	TObjectPtr<class UNiagaraSystem> MuzzleEffect;
	
	UPROPERTY(Replicated)
	TObjectPtr<class UNiagaraSystem> AoeEffect;
	
	friend APlacedTower;
	friend class UTowerUpgradeWidget;

};
