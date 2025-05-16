// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GamePawnComponent.generated.h"

class APolyPalsGamePawn;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POLYPALSDEFENSE_API UGamePawnComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGamePawnComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	APolyPalsGamePawn* GetGamePawn() const { return GamePawn; }

	void ServerSpawnGamepawn();
	
private:
	UPROPERTY()
	TSubclassOf<APolyPalsGamePawn> GamePawnClassDefault;
	UPROPERTY(Replicated)
	TObjectPtr<APolyPalsGamePawn> GamePawn;
};
