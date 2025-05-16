// Fill out your copyright notice in the Description page of Project Settings.


#include "Multiplayer/Components/PolyPalsGamePawn/BuildTowerComponent.h"

// Sets default values for this component's properties
UBuildTowerComponent::UBuildTowerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBuildTowerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UBuildTowerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UBuildTowerComponent::ClientOnInputTest()
{
	UE_LOG(LogTemp, Log, TEXT("UBuildTowerComponent detected input test"));
}

