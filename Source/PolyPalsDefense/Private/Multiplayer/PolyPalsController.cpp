// Fill out your copyright notice in the Description page of Project Settings.


#include "Multiplayer/PolyPalsController.h"
#include "Multiplayer/Components/PolyPalsInputComponent.h"
#include "Multiplayer/InputConfig.h"

APolyPalsController::APolyPalsController()
{
	PolyPalsInputComponent = CreateDefaultSubobject<UPolyPalsInputComponent>(TEXT("PolyPalsInputComponent"));
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
