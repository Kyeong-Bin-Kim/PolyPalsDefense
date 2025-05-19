// Fill out your copyright notice in the Description page of Project Settings.


#include "Multiplayer/Components/PolyPalsGamePawn/BuildTowerComponent.h"
#include "Multiplayer/PolyPalsGamePawn.h"
#include "Tower/PreviewBuilding.h"
#include "Tower/PlacedTower.h"
#include "Core/Subsystems/TowerDataManager.h"

#include "Net/UnrealNetwork.h"


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

void UBuildTowerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBuildTowerComponent, PlayerColor);
}

void UBuildTowerComponent::ClientSpawnPreviewBuilding()
{
	APolyPalsGamePawn* gamePawn = GetOwner<APolyPalsGamePawn>();
	if (!gamePawn) return;
	if (gamePawn->HasAuthority()) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = GetOwner()->GetInstigator();

	PreviewBuilding = GetWorld()->SpawnActor<APreviewBuilding>(APreviewBuilding::StaticClass(),
		FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	PreviewBuilding->ShowPreviewBuilding(false);
	PreviewBuilding->SetReplicates(false);
}

void UBuildTowerComponent::ServerSetPlayerColor(EPlayerColor InColor)
{
	if (!GetOwner()->HasAuthority()) return;
	PlayerColor = InColor;
}

void UBuildTowerComponent::ClientOnInputTest()
{
	UE_LOG(LogTemp, Log, TEXT("UBuildTowerComponent detected input test"));
	OnSelectTower(1);
}

void UBuildTowerComponent::ClientOnInputClick()
{
	UE_LOG(LogTemp, Log, TEXT("UBuildTowerComponent detected input click"));

	if (BuildState == EBuildState::SerchingPlace)
		SetBuildState(EBuildState::DecidePlacementLocation);
}

void UBuildTowerComponent::OnSelectTower(const uint8 InTowerId)
{
	TowerOnSerchingQue = InTowerId;
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
	if (PreviewBuilding->IsBuildable())
	{
		SetBuildState(EBuildState::None);
		FVector_NetQuantize placeLocation = PreviewBuilding->GetActorLocation();
		Server_RequestSpawnTower(placeLocation, TowerOnSerchingQue);
		TowerOnSerchingQue = 0;
		PreviewBuilding->ShowPreviewBuilding(false);
	}
}

void UBuildTowerComponent::Server_RequestSpawnTower_Implementation(const FVector_NetQuantize InLocation, uint8 InTargetTower)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	APlacedTower* Tower = GetWorld()->SpawnActor<APlacedTower>(APlacedTower::StaticClass(), InLocation, FRotator::ZeroRotator, SpawnParams);
	Tower->ExternalInitializeTower(InTargetTower, PlayerColor);
}