// Fill out your copyright notice in the Description page of Project Settings.


#include "Multiplayer/PolyPalsGameMode.h"
#include "Multiplayer/PolyPalsController.h"

void APolyPalsGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	uint8 Max = static_cast<uint8>(EPlayerColor::MAXCOLOR);
	for (uint8 Iter = 0; Iter < Max; Iter++)
	{
		EPlayerColor TargetColor = static_cast<EPlayerColor>(Iter);
		PreparedColors.Add(TargetColor);
	}

	PreparedColors.Remove(EPlayerColor::None);
	PreparedColors.Remove(EPlayerColor::MAXCOLOR);
}

void APolyPalsGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (!NewPlayer->IsLocalController())
	{
		UE_LOG(LogTemp, Log, TEXT("Client entered"));

		if (!PreparedColors.IsEmpty())
		{
			APolyPalsController* ProjectController = Cast<APolyPalsController>(NewPlayer);
			ProjectController->InitializeControllerDataByGameMode(PreparedColors[0]);
			PreparedColors.RemoveAt(0);
		}
	}
}
