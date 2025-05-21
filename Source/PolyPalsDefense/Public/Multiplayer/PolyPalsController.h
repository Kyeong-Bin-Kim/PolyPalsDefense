// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DataAsset/Tower/TowerEnums.h"
#include "PolyPalsController.generated.h"

class UPolyPalsInputComponent;
class UGamePawnComponent;

UCLASS()
class POLYPALSDEFENSE_API APolyPalsController : public APlayerController
{
	GENERATED_BODY()
	

public:
	APolyPalsController();

protected:
	virtual void SetupInputComponent() override;

	virtual void BeginPlay() override;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPolyPalsInputComponent* GetPolyPalsInputComponent() const { return PolyPalsInputComponent; }
	UGamePawnComponent* GetGamePawnComponent() const { return GamePawnComponent; }
	EPlayerColor GetPlayerColor() const { return PlayerColor; }

	void SetPlayerColor(EPlayerColor InColor);

private:
	void InitializeControllerDataByGameMode(EPlayerColor InColor);

protected:
	UPROPERTY()
	TObjectPtr<UPolyPalsInputComponent> PolyPalsInputComponent;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UGamePawnComponent> GamePawnComponent;

	UPROPERTY(Replicated)
	EPlayerColor PlayerColor = EPlayerColor::None;

	friend class APolyPalsGameMode;
};
