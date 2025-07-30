#include "PolyPalsController.h"
#include "PolyPalsGameMode.h"
#include "PolyPalsState.h"
#include "PolyPalsPlayerState.h"
#include "PolyPalsGamePawn.h"
#include "PolyPalsGameInstance.h"
#include "EnhancedInputComponent.h"
#include "PolyPalsController/GamePawnComponent.h"
#include "PolyPalsController/PolyPalsInputComponent.h"
#include "PolyPalsGamePawn/BuildTowerComponent.h"
#include "PolyPalsGamePawn/TowerHandleComponent.h"
#include "MainUIWidget.h"
#include "StageSelectUIWidget.h"
#include "LobbyUIWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Net/UnrealNetwork.h"

APolyPalsController::APolyPalsController()
{
	GamePawnComponent = CreateDefaultSubobject<UGamePawnComponent>(TEXT("GamePawnComponent"));
	GamePawnComponent->SetIsReplicated(true);
	PolyPalsInputComponent = CreateDefaultSubobject<UPolyPalsInputComponent>(TEXT("PolyPalsInputComponent"));
	PolyPalsInputComponent->SetIsReplicated(false);
}

void APolyPalsController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController())
		return;

	bShowMouseCursor = true;
	SetupInitialUIWidgets();

	const FString MapName = UGameplayStatics::GetCurrentLevelName(this, true);
	ENetMode NetMode = GetNetMode();

	if (MapName.Equals(TEXT("EmptyLevel")))
	{
		
		if (NetMode == NM_Client)
		{

			ShowLobbyUI();
		}
		else if (NetMode == NM_Standalone)
		{
			ShowMainUI();
		}
		else
		{
			HideAllUI();
		}
	}
	else
	{
		HideAllUI();
	}
}

void APolyPalsController::OnRep_Pawn()
{
	Super::OnRep_Pawn();

	if (!IsLocalController() || !GetPawn())
		return;

	if (UPolyPalsInputComponent* InputComp = FindComponentByClass<UPolyPalsInputComponent>())
	{
		APolyPalsGamePawn* MyPawn = Cast<APolyPalsGamePawn>(GetPawn());
		UBuildTowerComponent* BuildComp = MyPawn ? MyPawn->GetBuildTowerComponent() : nullptr;
		UTowerHandleComponent* HandleComp = MyPawn ? MyPawn->GetTowerHandleComponent() : nullptr;

		if (BuildComp && HandleComp)
		{
			InputComp->OnInputTower1.BindUObject(BuildComp, &UBuildTowerComponent::OnInputTower1);
			InputComp->OnInputTower2.BindUObject(BuildComp, &UBuildTowerComponent::OnInputTower2);
			InputComp->OnInputTower3.BindUObject(BuildComp, &UBuildTowerComponent::OnInputTower3);

			InputComp->OnInputRightClick.BindUObject(BuildComp, &UBuildTowerComponent::OnInputRightClick);

			InputComp->OnInputLeftClick.Clear();

			InputComp->OnInputLeftClick.AddUObject(BuildComp, &UBuildTowerComponent::OnInputLeftClick);
			InputComp->OnInputLeftClick.AddUObject(HandleComp, &UTowerHandleComponent::HandleLeftClick);
		}
	}

}

void APolyPalsController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (!IsLocalController()) return;

	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	if (UPolyPalsInputComponent* InputComp = FindComponentByClass<UPolyPalsInputComponent>())
	{
		InputComp->OnInputTower1.Unbind();
		InputComp->OnInputTower2.Unbind();
		InputComp->OnInputTower3.Unbind();
		InputComp->OnInputRightClick.Unbind();

		InputComp->OnInputLeftClick.Clear();

		InputComp->SetupEnhancedInput(this);
	}

	bShowMouseCursor = true;

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
}

void APolyPalsController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(APolyPalsController, PlayerColor);
}

void APolyPalsController::InitializeAndShowLobbyUI(FName InStageName, const FString& HostName)
{
	ShowLobbyUI();
	ConfigureLobbyUI(InStageName, HostName);
	RefreshLobbyUI();
}

void APolyPalsController::Server_SetReady_Implementation(bool bReady)
{
	APolyPalsPlayerState* MyState = GetPlayerState<APolyPalsPlayerState>();

	if (MyState)
	{
		MyState->SetReadyState(bReady);

		APolyPalsState* GameState = GetWorld()->GetGameState<APolyPalsState>();

		if (GameState)
		{
			GameState->UpdateReadyPlayers();
		}
	}
}

void APolyPalsController::SetPlayerColor(EPlayerColor InColor)
{
	uint8 EnumToInt = static_cast<uint8>(InColor);

	PlayerColor = InColor; 
}

void APolyPalsController::SetLobbyUIInstance(ULobbyUIWidget* InWidget)
{
	LobbyUIInstance = InWidget;
}

void APolyPalsController::UpdatePlayerNickname(const FString& NewName)
{
	if (APolyPalsPlayerState* PS = GetPlayerState<APolyPalsPlayerState>())
	{
		PS->SetPlayerName(NewName);
	}

	if (UMainUIWidget* Widget = Cast<UMainUIWidget>(MainUIWidgetInstance))
	{
		Widget->SetPlayerNameText(NewName);
	}
}

void APolyPalsController::UpdateReadyUI(APolyPalsPlayerState* ChangedPlayerState, bool bIsReady)
{
	if (LobbyUIInstance)
	{
		LobbyUIInstance->UpdatePlayerSlotReadyState(ChangedPlayerState, bIsReady);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("LobbyUIInstance is null in UpdateReadyUI!"));
	}
}

void APolyPalsController::RefreshLobbyUI()
{
	if (!IsLocalController() || !LobbyUIInstance)
	{
		return;
	}

	if (APolyPalsState* GS = GetWorld()->GetGameState<APolyPalsState>())
	{
		LobbyUIInstance->SetRoomTitle(GS->GetLobbyName());
		LobbyUIInstance->SetSelectedStage(GS->GetSelectedStage());

		LobbyUIInstance->SetRoomTitle(GS->GetLobbyName());
		LobbyUIInstance->SetSelectedStage(GS->GetSelectedStage());

		LobbyUIInstance->UpdateLobbyInfo(GS->GetConnectedPlayers(), GS->GetReadyPlayers(), GS->GetSelectedStage(), HasAuthority());

		LobbyUIInstance->RefreshPlayerSlots(GS->PlayerArray);
	}
}

void APolyPalsController::HideAllUI()
{
	if (MainUIWidgetInstance) MainUIWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
	if (StageSelectWidgetInstance) StageSelectWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
	if (LobbyUIInstance) LobbyUIInstance->SetVisibility(ESlateVisibility::Hidden);
}

void APolyPalsController::SetupInitialUIWidgets()
{
	if (!MainUIWidgetInstance && MainUIWidgetClass)
	{
		MainUIWidgetInstance = CreateWidget<UMainUIWidget>(this, MainUIWidgetClass);
		if (MainUIWidgetInstance)
		{
			MainUIWidgetInstance->AddToViewport();
			MainUIWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (!StageSelectWidgetInstance && StageSelectWidgetClass)
	{
		StageSelectWidgetInstance = CreateWidget<UStageSelectUIWidget>(this, StageSelectWidgetClass);
		if (StageSelectWidgetInstance)
		{
			StageSelectWidgetInstance->AddToViewport();
			StageSelectWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (!LobbyUIInstance && LobbyUIWidgetClass)
	{
		LobbyUIInstance = CreateWidget<ULobbyUIWidget>(this, LobbyUIWidgetClass);
		if (LobbyUIInstance)
		{
			LobbyUIInstance->AddToViewport();
			LobbyUIInstance->SetVisibility(ESlateVisibility::Hidden);
			SetLobbyUIInstance(LobbyUIInstance);
		}
	}
}

void APolyPalsController::ShowMainUI()
{
	SetupInitialUIWidgets();
	HideAllUI();

	if (UMainUIWidget* TypedWidget = Cast<UMainUIWidget>(MainUIWidgetInstance))
	{
		FString PlayerName = TEXT("Player");

		if (IOnlineSubsystem* OSS = IOnlineSubsystem::Get())
		{
			IOnlineIdentityPtr Identity = OSS->GetIdentityInterface();

			if (Identity.IsValid())
			{
				TSharedPtr<const FUniqueNetId> UserId = Identity->GetUniquePlayerId(0);

				if (UserId.IsValid())
				{
					PlayerName = Identity->GetPlayerNickname(*UserId);
				}
				else if (UPolyPalsGameInstance* GI = Cast<UPolyPalsGameInstance>(GetGameInstance()))
				{
					GI->LoginToSteam();
				}
			}
		}
		TypedWidget->SetPlayerNameText(PlayerName);
	}

	if (MainUIWidgetInstance)
	{
		MainUIWidgetInstance->SetVisibility(ESlateVisibility::Visible);
	}
}

void APolyPalsController::ShowStageSelectUI()
{
	SetupInitialUIWidgets();
	HideAllUI();

	if (StageSelectWidgetInstance)
	{
		StageSelectWidgetInstance->SetVisibility(ESlateVisibility::Visible);
	}
}

void APolyPalsController::ShowLobbyUI()
{
	SetupInitialUIWidgets();
	HideAllUI();

	if (LobbyUIInstance)
	{
		LobbyUIInstance->SetVisibility(ESlateVisibility::Visible);
		RefreshLobbyUI();
	}
}

void APolyPalsController::ConfigureLobbyUI(FName InStageName, const FString& HostName)
{
	if (!LobbyUIInstance)
		return;

	LobbyUIInstance->SetSelectedStage(InStageName);
	LobbyUIInstance->SetRoomTitle(HostName);
}

void APolyPalsController::LeaveLobby()
{
	ShowMainUI();
}

void APolyPalsController::BeginSelectTower(int32 TowerIndex)
{
	PendingTowerIndex = TowerIndex;

	if (APolyPalsGamePawn* MyPawn = Cast<APolyPalsGamePawn>(GetPawn()))
	{
		if (UBuildTowerComponent* BuildComp = MyPawn->GetBuildTowerComponent())
		{
			BuildComp->ClientBeginSelectTower(static_cast<uint8>(TowerIndex));
		}
	}
}

bool APolyPalsController::Server_BuildTower_Validate(int32 TowerIndex, FVector_NetQuantize InSpawnLocation)
{
	return TowerIndex >= 1 && TowerIndex <= 3;
}

void APolyPalsController::Server_BuildTower_Implementation(int32 TowerIndex, FVector_NetQuantize InSpawnLocation)
{
	APolyPalsGamePawn * MyPawn = Cast<APolyPalsGamePawn>(GetPawn());

	if (!MyPawn) return;
	
	UBuildTowerComponent * BuildComp = MyPawn->GetBuildTowerComponent();

	if (BuildComp)
	{
		BuildComp->Server_RequestSpawnTower(InSpawnLocation, TowerIndex);
	}
}

void APolyPalsController::Server_RequestReturnToLobby_Implementation()
{
	if (APolyPalsGameMode* GM = GetWorld()->GetAuthGameMode<APolyPalsGameMode>())
	{
		GM->ResetAndReturnToLobby();
	}
}

void APolyPalsController::InitializeControllerDataByGameMode(EPlayerColor InColor)
{
	PlayerColor = InColor;

	if (GamePawnComponent->GetGamePawn())
	{
		UBuildTowerComponent* BuildTowerComp = GamePawnComponent->GetGamePawn()->GetBuildTowerComponent();
		BuildTowerComp->SetPlayerColor(PlayerColor);
	}
}