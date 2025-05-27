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
	// Sets default values for this pawn's properties
	APolyPalsGamePawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	inline bool IsPossessed() const { return bIsPossessed; }
	UBuildTowerComponent* GetBuildTowerComponent() const { return BuildTowerComponent; }
	APolyPalsController* GetPossessedController() const { return PolyPalsController; }

private:
	void UnbindInputDelegate();

	UFUNCTION()
	void OnRep_PolyPalsController();

protected:
	UPROPERTY(EditDefaultsOnly)
	FVector SpectateLocation;

private:
	UPROPERTY(Replicated)
	bool bIsPossessed = false;

private:
	// Cach data
	UPROPERTY(ReplicatedUsing = OnRep_PolyPalsController)
	TObjectPtr<APolyPalsController> PolyPalsController;

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
	UPROPERTY()
	TObjectPtr<UBuildTowerComponent> BuildTowerComponent;
	UPROPERTY()
	TObjectPtr<UTowerHandleComponent> TowerHandleComponent;

	friend UBuildTowerComponent;

};
