// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DataAsset/Tower/TowerEnums.h"
#include "PolyPalsGameMode.generated.h"

/**
 * 
 */
UCLASS()
class POLYPALSDEFENSE_API APolyPalsGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage);
	virtual void PostLogin(APlayerController* NewPlayer);

private:
	UPROPERTY()
	TArray<EPlayerColor> PreparedColors;
};
