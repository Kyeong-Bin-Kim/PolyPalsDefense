// Fill out your copyright notice in the Description page of Project Settings.


#include "Multiplayer/Components/PolyPalsGamePawn/BuildTowerComponent.h"
#include "Multiplayer/PolyPalsGamePawn.h"
#include "Tower/PreviewBuilding.h"
#include "Core/Subsystems/TowerDataManager.h"


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

	TowerDataManager = GetWorld()->GetSubsystem<UTowerDataManager>();
	ClientSpawnPreviewBuilding();
}


// Called every frame
void UBuildTowerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UBuildTowerComponent::ClientSpawnPreviewBuilding()
{
	bool bIsLocalComponent = false;

	APolyPalsGamePawn* gamePawn = GetOwner<APolyPalsGamePawn>();
	if (!gamePawn) return;
	if (gamePawn->HasAuthority()) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	PreviewBuilding = GetWorld()->SpawnActor<APreviewBuilding>(APreviewBuilding::StaticClass(),
		FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	PreviewBuilding->ShowPreviewBuilding(false);
}

void UBuildTowerComponent::ClientOnInputTest()
{
	UE_LOG(LogTemp, Log, TEXT("UBuildTowerComponent detected input test"));
	OnSelectTower(1);
}

void UBuildTowerComponent::ClientOnInputClick()
{
	UE_LOG(LogTemp, Log, TEXT("UBuildTowerComponent detected input click"));
}

void UBuildTowerComponent::OnSelectTower(const uint8 InTowerId)
{
	PreviewBuilding->ShowPreviewBuilding(true, InTowerId);
	SetBuildState(EBuildState::SerchingPlace);
}

void UBuildTowerComponent::SetBuildState(EBuildState InState)
{
	BuildState = InState;
	switch (InState)
	{
	case EBuildState::None:
		OnNormal();
		break;
	case EBuildState::SerchingPlace:
		OnSerchingPlace();
		break;
	case EBuildState::DecidePlacementLocation:
		OnDecidePlacementLocation();
		break;
	}
}

void UBuildTowerComponent::OnNormal()
{
}

void UBuildTowerComponent::OnSerchingPlace()
{
}

void UBuildTowerComponent::OnDecidePlacementLocation()
{
}

