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
	UGamePawnComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	APolyPalsGamePawn* GetGamePawn() const { return GamePawn; }

	void ServerSpawnGamepawn();
	
private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APolyPalsGamePawn> GamePawnClassDefault;

	UPROPERTY(Replicated)
	TObjectPtr<APolyPalsGamePawn> GamePawn;
};
