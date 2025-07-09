// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataAsset/Tower/TowerEnums.h"
#include "PlacedTower.generated.h"

class UBoxComponent;
class UWidgetComponent;
class UNiagaraComponent;
class USphereComponent;
class UTowerAttackComponent;
class UTowerUpgradeComponent;
class UTowerUpgradeWidget;
class APolyPalsController;

UCLASS()
class POLYPALSDEFENSE_API APlacedTower : public AActor
{
	GENERATED_BODY()

public:
	APlacedTower();

protected:
	virtual void BeginPlay() override;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void ExternalInitializeTower(uint8 InTowerId, EPlayerColor InColor, APolyPalsController* const InController);

	UFUNCTION()
	void OnRep_PlayerColor();

	UFUNCTION()
	void OnRep_TowerId();

	void ClientSetupTowerWidgetComponent();

	void ClientSetTowerMeshComponent(uint8 InTowerId, EPlayerColor InColor);

	UTowerAttackComponent* GetAttackComponent() const { return TowerAttackComponent; }

	void SetWidgetHidden(bool bIsDeactice);

	UFUNCTION()
	void UpgradeTower();

	void UpdateLevel();

private:
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void SetTowerCollision();
	void SetupVisuals();

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> RootBoxComponent;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> TowerMeshComponent;

	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> GunMeshComponent;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UWidgetComponent> TowerUpWidgetComponent;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraComponent> MuzzleEffectComponent;

	UPROPERTY()
	TObjectPtr<USphereComponent> TowerRangeSphere;

	UPROPERTY()
	TObjectPtr<UTowerAttackComponent> TowerAttackComponent;

	UPROPERTY()
	TSubclassOf<UTowerUpgradeWidget> TowerUpWidgetClass;

	
	UPROPERTY(Replicated)
	TObjectPtr<APolyPalsController> OwnerController;
	
	UPROPERTY(ReplicatedUsing = OnRep_PlayerColor)
	EPlayerColor PlayerColor = EPlayerColor::None;

	UPROPERTY(EditInstanceOnly, ReplicatedUsing = OnRep_TowerId)
	int16 TowerId = -1;

	UPROPERTY(EditDefaultsOnly)
	FName AttackTargetTag = FName("Enemy");

	FTimerHandle VisualSetupHandle;

	friend UTowerAttackComponent;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TObjectPtr<class UWidgetComponent> LevelWidgetComponent;

	UPROPERTY(EditAnywhere, Category = "Upgrade")
	int32 Level = 1;

	UPROPERTY(EditAnywhere, Category = "Upgrade")
	int32 MaxLevel = 3;

	UPROPERTY(EditAnywhere, Category = "Upgrade")
	int32 UpgradeCost = 500;


};
