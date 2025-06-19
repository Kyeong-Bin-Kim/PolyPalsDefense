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
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSubsystemTypes.h" 
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

		const FString CurrentMapName = UGameplayStatics::GetCurrentLevelName(this, true);
		if (CurrentMapName.Contains(TEXT("EmptyLevel")))
		{
			ShowMainUI();
		}
	}
}

void APolyPalsController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(APolyPalsController, PlayerColor);
}

void APolyPalsController::Server_SetSelectedStage_Implementation(FName StageName)
{
	if (APolyPalsState* GS = GetWorld()->GetGameState<APolyPalsState>())
	{
		GS->SetSelectedStage(StageName);
	}
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
			// 로컬 플레이어 이름 설정 (디폴트)
			FString PlayerName = TEXT("Player");

			TypedWidget->SetPlayerNameText(PlayerName);

			// 2. Steam 연동된 경우: OSS 통해 가져오기 가능
			// (별도 연동 필요: Steam OSS)
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
				}
			}

			TypedWidget->SetPlayerNameText(PlayerName);
		}

		MainUIWidgetInstance->AddToViewport();
	}
}