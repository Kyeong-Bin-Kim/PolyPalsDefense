// Fill out your copyright notice in the Description page of Project Settings.


#include "Multiplayer/PolyPalsController.h"
#include "Multiplayer/PolyPalsGamePawn.h"
#include "Multiplayer/Components/PolyPalsController/PolyPalsInputComponent.h"
#include "Multiplayer/Components/PolyPalsController/GamePawnComponent.h"
#include "Multiplayer/InputConfig.h"

#include "Net/UnrealNetwork.h"

APolyPalsController::APolyPalsController()
{
	PolyPalsInputComponent = CreateDefaultSubobject<UPolyPalsInputComponent>(TEXT("PolyPalsInputComponent"));
	GamePawnComponent = CreateDefaultSubobject<UGamePawnComponent>(TEXT("GamePawnComponent"));
	GamePawnComponent->SetIsReplicated(true);
}

void APolyPalsController::SetupInputComponent()
{
	Super::SetupInputComponent();
	PolyPalsInputComponent->SetupEnhancedInput(this);
}

void APolyPalsController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
		bShowMouseCursor = true;
}

void APolyPalsController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(APolyPalsController, PlayerColor);
}

void APolyPalsController::SetPlayerColor(EPlayerColor InColor) { 
	uint8 EnumToInt = static_cast<uint8>(InColor);
	UE_LOG(LogTemp, Log, TEXT("APolyPalsController: Player color set to: %d"), EnumToInt);
	PlayerColor = InColor; 

}
