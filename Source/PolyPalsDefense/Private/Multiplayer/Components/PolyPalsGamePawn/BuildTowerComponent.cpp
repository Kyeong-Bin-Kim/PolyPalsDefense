#include "Components/PolyPalsGamePawn/BuildTowerComponent.h"
#include "Components/PolyPalsGamePawn/TowerHandleComponent.h"
#include "PolyPalsGamePawn.h"
#include "PolyPalsController.h"
#include "PolyPalsPlayerState.h"
#include "Tower/TowerPropertyData.h"
#include "PreviewBuilding.h"
#include "PlacedTower.h"
#include "Subsystems/TowerDataManager.h"

#include "UI/TowerUpgradeWidget.h" 
#include "Blueprint/UserWidget.h"

#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerController.h"


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

<<<<<<< HEAD
=======
void UBuildTowerComponent::ClientOnInputTest()
{
	UE_LOG(LogTemp, Log, TEXT("UBuildTowerComponent detected input test"));
	
}
>>>>>>> UI
void UBuildTowerComponent::ClientOnInputClick()
{
	//UE_LOG(LogTemp, Log, TEXT("UBuildTowerComponent detected input click"));

	if (BuildState == EBuildState::SerchingPlace)
	{
		SetBuildState(EBuildState::DecidePlacementLocation);
		return;
	}

	// GetOwner는 AActor*이므로 반드시 APawn으로 캐스팅 후 GetController
	APawn* PawnOwner = Cast<APawn>(GetOwner());
	if (!PawnOwner) return;

	APlayerController* PC = Cast<APlayerController>(PawnOwner->GetController());
	if (!PC) return;

	// 마우스 아래 액터 추적
	FHitResult HitResult;
	PC->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), false, HitResult);

	APlacedTower* ClickedTower = Cast<APlacedTower>(HitResult.GetActor());
	if (ClickedTower)
	{
		AController* LocalController = PC;
		if (ClickedTower->GetOwner() && ClickedTower->GetOwner() != LocalController)
		{
			UE_LOG(LogTemp, Warning, TEXT("타워의 오너가 로컬 컨트롤러와 다릅니다. UI 생략"));
			return;
		}
		UE_LOG(LogTemp, Log, TEXT("PlacedTower Clicked: %s"), *ClickedTower->GetName());

		// 업그레이드 위젯 생성
		if (TowerUpgradeWidgetClass)
		{
			if (UpgradeWidgetInstance)
			{
				UpgradeWidgetInstance->RemoveFromParent();
				UpgradeWidgetInstance = nullptr;
			}

			UpgradeWidgetInstance = CreateWidget<UTowerUpgradeWidget>(PC, TowerUpgradeWidgetClass);
			if (UpgradeWidgetInstance)
			{
				UpgradeWidgetInstance->SetTargetTower(ClickedTower);
				UpgradeWidgetInstance->AddToViewport();
			}
		}
	}
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

	APolyPalsPlayerState* PolypalsPlayerState = GamePawn->GetController<APolyPalsController>()->GetPlayerState<APolyPalsPlayerState>();
	PolypalsPlayerState->AddGold(-Require);
}