#include "PolyPalsController.h"
#include "PolyPalsPlayerState.h"
#include "PolyPalsState.h"
#include "PolyPalsGamePawn.h"
#include "PolyPalsController/PolyPalsInputComponent.h"
#include "PolyPalsController/GamePawnComponent.h"
#include "PolyPalsGamePawn/BuildTowerComponent.h"
#include "MainUIWidget.h"
#include "LobbyUIWidget.h"
#include "InputConfig.h"

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
	{
		bShowMouseCursor = true;
		ShowMainUI();
	}
}

void APolyPalsController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(APolyPalsController, PlayerColor);
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

void APolyPalsController::SetPlayerColor(EPlayerColor InColor) { 
	uint8 EnumToInt = static_cast<uint8>(InColor);
	UE_LOG(LogTemp, Log, TEXT("APolyPalsController: Player color set to: %d"), EnumToInt);
	PlayerColor = InColor; 

}

void APolyPalsController::SetLobbyUIInstance(ULobbyUIWidget* InWidget)
{
	LobbyUIInstance = InWidget;
}

void APolyPalsController::UpdateReadyUI(APolyPalsPlayerState* ChangedPlayerState, bool bIsReady)
{
	if (LobbyUIInstance)
	{
		LobbyUIInstance->UpdatePlayerSlotReadyState(ChangedPlayerState, bIsReady);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("LobbyUIInstance가 없습니다. Ready 상태를 UI에 반영할 수 없습니다."));
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
		LobbyUIInstance->UpdateLobbyInfo(GS->GetConnectedPlayers(), GS->GetReadyPlayers(), GS->GetSelectedStage(), HasAuthority());
		LobbyUIInstance->RefreshPlayerSlots(GS->PlayerArray);
	}
}

void APolyPalsController::InitializeControllerDataByGameMode(EPlayerColor InColor)
{
	PlayerColor = InColor;

	if (GamePawnComponent->GetGamePawn())
	{
		UBuildTowerComponent* BuildTowerComp = GamePawnComponent->GetGamePawn()->GetBuildTowerComponent();
		BuildTowerComp->SetPlayerColorByController(PlayerColor);
	}
}

void APolyPalsController::ShowMainUI()
{
	if (!MainUIWidgetInstance && MainUIWidgetClass)
	{
		MainUIWidgetInstance = CreateWidget<UMainUIWidget>(this, MainUIWidgetClass);

		if (UMainUIWidget* TypedWidget = Cast<UMainUIWidget>(MainUIWidgetInstance))
		{
			FString PlayerName;

			// 기본 값
			TypedWidget->SetPlayerNameText(TEXT("Player"));

			// 2. Steam 연동된 경우: OSS 통해 가져오기 가능
			// (별도 연동 필요: Steam OSS)

			//TypedWidget->SetPlayerNameText(PlayerName);
		}

		MainUIWidgetInstance->AddToViewport();
	}
}