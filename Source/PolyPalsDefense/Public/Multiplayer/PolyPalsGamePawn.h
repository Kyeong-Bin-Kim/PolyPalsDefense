// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PolyPalsGamePawn.generated.h"

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

private:
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
	TObjectPtr<class APolyPalsController> PolyPalsController;

private:
	// Components
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class USceneComponent> RootSceneComponent;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class USpringArmComponent> SpringArm;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UCameraComponent> PolyPalsPlayCamera;
	
private:
	// ActorComponents
	UPROPERTY()
	TObjectPtr<class UBuildTowerComponent> BuildTowerComponent;

};
