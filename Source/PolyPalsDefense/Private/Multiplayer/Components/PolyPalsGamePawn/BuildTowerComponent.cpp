#include "Components/PolyPalsGamePawn/BuildTowerComponent.h"
#include "Components/PolyPalsGamePawn/TowerHandleComponent.h"
#include "PolyPalsGamePawn.h"
#include "PolyPalsController.h"
#include "PolyPalsPlayerState.h"
#include "Tower/TowerPropertyData.h"
#include "PreviewBuilding.h"
#include "PlacedTower.h"
#include "Subsystems/TowerDataManager.h"

#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
UBuildTowerComponent::UBuildTowerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;

	ConstructorHelpers::FClassFinder<APlacedTower> TowerClass(TEXT("/Game/Blueprints/Towers/BP_PlacedTower.BP_PlacedTower_C"));
	if (TowerClass.Succeeded())
		PlacedTowerBlueClass = TowerClass.Class;
}


// Called when the game starts
void UBuildTowerComponent::BeginPlay()
{
	Super::BeginPlay();

	TowerDataManager = GetWorld()->GetSubsystem<UTowerDataManager>();
	ClientSpawnPreviewBuilding();
}


void UBuildTowerComponent::InitializeComponent()
{
	Super::InitializeComponent();

	GamePawn = GetOwner<APolyPalsGamePawn>();
	TowerHandleComponent = GamePawn->TowerHandleComponent;
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
	if (!GamePawn) return;
	if (GamePawn->HasAuthority()) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = GetOwner()->GetInstigator();

	PreviewBuilding = GetWorld()->SpawnActor<APreviewBuilding>(APreviewBuilding::StaticClass(),
		FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	PreviewBuilding->ShowPreviewBuilding(false);
	PreviewBuilding->SetReplicates(false);
}

void UBuildTowerComponent::SetPlayerColorByController(EPlayerColor InColor) { PlayerColor = InColor; }

void UBuildTowerComponent::ClientOnInputTest()
{
	//UE_LOG(LogTemp, Log, TEXT("UBuildTowerComponent detected input test"));
	
}

void UBuildTowerComponent::ClientOnInputClick()
{
	//UE_LOG(LogTemp, Log, TEXT("UBuildTowerComponent detected input click"));

	if (BuildState == EBuildState::SerchingPlace)
		SetBuildState(EBuildState::DecidePlacementLocation);
}

void UBuildTowerComponent::ClientOnInputRightClick()
{
	if (BuildState == EBuildState::SerchingPlace)
		SetBuildState(EBuildState::None);
}

void UBuildTowerComponent::ClientOnInputTower1()
{
	if (BuildState == EBuildState::None)
	{
		UTowerPropertyData* Data = GetWorld()->GetSubsystem<UTowerDataManager>()->GetTowerPropertyData(1);
		FTowerUpgradeValue* UpgradeData = Data->UpgradeData.Find(ELevelValue::Level1);
		int32 Require = UpgradeData->Cost;

		int32 CurrentGold = 0;

		if (const APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			if (const APolyPalsPlayerState* PlayerState = PC->GetPlayerState<APolyPalsPlayerState>())
			{
				CurrentGold = PlayerState->GetPlayerGold();
			}
		}

		if (CurrentGold >= Require)
			OnSelectTower(1);
		else
			TryBuildButNotEnoughGold.ExecuteIfBound();
	}
}

void UBuildTowerComponent::ClientOnInputTower2()
{
	if (BuildState == EBuildState::None)
	{
		UTowerPropertyData* Data = GetWorld()->GetSubsystem<UTowerDataManager>()->GetTowerPropertyData(2);
		FTowerUpgradeValue* UpgradeData = Data->UpgradeData.Find(ELevelValue::Level1);
		int32 Require = UpgradeData->Cost;

		int32 CurrentGold = 0;

		if (const APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			if (const APolyPalsPlayerState* PlayerState = PC->GetPlayerState<APolyPalsPlayerState>())
			{
				CurrentGold = PlayerState->GetPlayerGold();
			}
		}

		if (CurrentGold >= Require)
			OnSelectTower(2);
		else
			TryBuildButNotEnoughGold.ExecuteIfBound();
	}
}

void UBuildTowerComponent::ClientOnInputTower3()
{
	if (BuildState == EBuildState::None)
	{
		UTowerPropertyData* Data = GetWorld()->GetSubsystem<UTowerDataManager>()->GetTowerPropertyData(3);
		FTowerUpgradeValue* UpgradeData = Data->UpgradeData.Find(ELevelValue::Level1);
		int32 Require = UpgradeData->Cost;
		
		int32 CurrentGold = 0;

		if (const APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			if (const APolyPalsPlayerState* PlayerState = PC->GetPlayerState<APolyPalsPlayerState>())
			{
				CurrentGold = PlayerState->GetPlayerGold();
			}
		}

		if (CurrentGold >= Require)
			OnSelectTower(3);
		else
			TryBuildButNotEnoughGold.ExecuteIfBound();
	}
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
	TowerHandleComponent->SetBuildState(EBuildState::None);
	TowerOnSerchingQue = 0;
	PreviewBuilding->ShowPreviewBuilding(false);
}

void UBuildTowerComponent::OnSerchingPlace()
{
	TowerHandleComponent->SetBuildState(EBuildState::SerchingPlace);
}

void UBuildTowerComponent::OnDecidePlacementLocation()
{
	TowerHandleComponent->SetBuildState(EBuildState::DecidePlacementLocation);

	if (PreviewBuilding->IsBuildable())
	{
		FVector_NetQuantize placeLocation = PreviewBuilding->GetActorLocation();
		Server_RequestSpawnTower(placeLocation, TowerOnSerchingQue);
		SetBuildState(EBuildState::None);
	}
	else
		SetBuildState(EBuildState::SerchingPlace);
}

void UBuildTowerComponent::Server_RequestSpawnTower_Implementation(const FVector_NetQuantize InLocation, uint8 InTargetTower)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	FVector SpawnLocation = InLocation;
	SpawnLocation.Z += 80.f;

	APlacedTower* Tower = GetWorld()->SpawnActor<APlacedTower>(PlacedTowerBlueClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
	APolyPalsController* OwnerController = GamePawn->GetPossessedController();
	Tower->ExternalInitializeTower(InTargetTower, PlayerColor, OwnerController);

	UTowerPropertyData* Data = GetWorld()->GetSubsystem<UTowerDataManager>()->GetTowerPropertyData(InTargetTower);
	FTowerUpgradeValue* UpgradeData = Data->UpgradeData.Find(ELevelValue::Level1);
	int32 Require = UpgradeData->Cost;
	GetWorld()->GetGameState<APolyPalsPlayerState>()->AddGold(-Require);
	//int32 Current = GetWorld()->GetGameState<APolyPalsState>()->GetGold();
	//UE_LOG(LogTemp, Log, TEXT("Gold: %d"), Current)

}