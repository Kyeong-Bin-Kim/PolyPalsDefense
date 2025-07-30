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
	UE_LOG(LogTemp, Log, TEXT("APolyPalsController::BeginPlay - MapName: %s, NetMode: %d"), *MapName, static_cast<int32>(NetMode));

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

	UE_LOG(LogTemp, Warning, TEXT("[Controller] OnRep_Pawn: ?몃━寃뚯씠??諛붿씤???쒖옉"));

	if (UPolyPalsInputComponent* InputComp = FindComponentByClass<UPolyPalsInputComponent>())
	{
		APolyPalsGamePawn* MyPawn = Cast<APolyPalsGamePawn>(GetPawn());
		UBuildTowerComponent* BuildComp = MyPawn ? MyPawn->GetBuildTowerComponent() : nullptr;
		UTowerHandleComponent* HandleComp = MyPawn ? MyPawn->GetTowerHandleComponent() : nullptr;

		if (BuildComp && HandleComp)
		{
			// 1,2,3 ?????꾨━酉?
			InputComp->OnInputTower1.BindUObject(BuildComp, &UBuildTowerComponent::OnInputTower1);
			InputComp->OnInputTower2.BindUObject(BuildComp, &UBuildTowerComponent::OnInputTower2);
			InputComp->OnInputTower3.BindUObject(BuildComp, &UBuildTowerComponent::OnInputTower3);

			// ?고겢由????꾨━酉?痍⑥냼
			InputComp->OnInputRightClick.BindUObject(BuildComp, &UBuildTowerComponent::OnInputRightClick);

			// 醫뚰겢由?
			InputComp->OnInputLeftClick.Clear();

			//   - 鍮뚮뱶 紐⑤뱶?????ㅼ튂 ?뺤젙
			InputComp->OnInputLeftClick.AddUObject(BuildComp, &UBuildTowerComponent::OnInputLeftClick);

			//   - 鍮뚮뱶 紐⑤뱶 ?꾨땺 ??湲곗〈 ????대┃ 泥섎━
			InputComp->OnInputLeftClick.AddUObject(HandleComp, &UTowerHandleComponent::HandleLeftClick);

			UE_LOG(LogTemp, Warning, TEXT("[Controller] OnRep_Pawn: ?몃━寃뚯씠??諛붿씤???꾨즺"));
		}
	}

}

void APolyPalsController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (!IsLocalController()) return;

	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	UE_LOG(LogTemp, Warning, TEXT(">>> SetupInputComponent called, has UPolyPalsInputComponent=%s"), FindComponentByClass<UPolyPalsInputComponent>() ? TEXT("YES") : TEXT("NO"));

	// EnhancedInput ?명똿
	if (UPolyPalsInputComponent* InputComp = FindComponentByClass<UPolyPalsInputComponent>())
	{
		// 諛붿씤??以묐났 諛⑹?
		// ?깃?罹먯뒪???몃━寃뚯씠?몃뒗 Unbind()
		InputComp->OnInputTower1.Unbind();
		InputComp->OnInputTower2.Unbind();
		InputComp->OnInputTower3.Unbind();
		InputComp->OnInputRightClick.Unbind();

		// 硫?곗틦?ㅽ듃 ?몃━寃뚯씠?몃뒗 Clear()
		InputComp->OnInputLeftClick.Clear();

		// ?μ긽???낅젰 ?뗮똿
		InputComp->SetupEnhancedInput(this);
	}

	// 而ㅼ꽌 蹂댁씠寃?
	bShowMouseCursor = true;

	// 寃뚯엫 & UI 紐⑤뱶 ?ㅼ젙
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
	ShowLobbyUI();								// UI 蹂댁씠湲?+ RefreshLobbyUI
	ConfigureLobbyUI(InStageName, HostName);	// 諛??쒕ぉ, ?ㅽ뀒?댁? ?명똿
	RefreshLobbyUI();							// ?レ옄쨌?щ’ ?ш갚??
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
		UE_LOG(LogTemp, Warning, TEXT("LobbyUIInstance媛 ?놁뒿?덈떎. Ready ?곹깭瑜?UI??諛섏쁺?????놁뒿?덈떎."));
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

		// 諛??쒕ぉ, ?ㅽ뀒?댁???媛깆떊
		LobbyUIInstance->SetRoomTitle(GS->GetLobbyName());
		LobbyUIInstance->SetSelectedStage(GS->GetSelectedStage());

		// 湲곗〈 濡쒖쭅: ?묒냽/以鍮??? ?щ’ 媛깆떊
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
			UE_LOG(LogTemp, Log, TEXT("PolyPalsController BeginSelectTower TowerIndex = %d"), TowerIndex);

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