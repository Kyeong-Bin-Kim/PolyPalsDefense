#include "PolyPalsGameMode.h"
#include "PolyPalsState.h"
#include "PolyPalsPlayerState.h"
#include "PolyPalsController.h"
#include "PolyPalsGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GenericPlatform/GenericPlatformHttp.h"

APolyPalsGameMode::APolyPalsGameMode()
{
	ConnectedPlayers = 0;

	StageMapPaths.Add(TEXT("Dirt"), TEXT("DirtStage"));
	StageMapPaths.Add(TEXT("Snow"), TEXT("SnowStage"));
}

void APolyPalsGameMode::BeginPlay()
{
	const FString Map = UGameplayStatics::GetCurrentLevelName(GetWorld(), true);

	UE_LOG(LogTemp, Log, TEXT("BeginPlay:Map-%s"), *Map);

	if (Map.Equals(TEXT("EmptyLevel"), ESearchCase::IgnoreCase))
	{
		if (UPolyPalsGameInstance* GI = Cast<UPolyPalsGameInstance>(GetGameInstance()))
		{
			SetMaxPlayerSlots(GI->GetMaxPlayerCount());

			UE_LOG(LogTemp, Log, TEXT("GameMode: MaxPlayerSlots set from GameInstance = %d"), MaxPlayerSlots);

			if (APolyPalsState* GS = GetGameState<APolyPalsState>())
			{
				if (GI->GetPendingStageName() != NAME_None)
				{
					GS->SetSelectedStage(GI->GetPendingStageName());
				}

				const FString& SavedName = GI->GetPendingLobbyName();

				if (!SavedName.IsEmpty())
				{
					GS->SetLobbyName(SavedName);
				}
			}
		}
	}
}

void APolyPalsGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	uint8 Max = static_cast<uint8>(EPlayerColor::MAXCOLOR);

	for (uint8 Iter = 0; Iter < Max; Iter++)
	{
		EPlayerColor TargetColor = static_cast<EPlayerColor>(Iter);
		PreparedColors.Add(TargetColor);
	}

	PreparedColors.Remove(EPlayerColor::None);
	PreparedColors.Remove(EPlayerColor::MAXCOLOR);
}

void APolyPalsGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	UE_LOG(LogTemp, Log, TEXT("[PreLogin] Options=%s"), *Options);

	if (ConnectedPlayers >= MaxPlayerSlots)
	{
		ErrorMessage = TEXT("Lobby is full.");
		UE_LOG(LogTemp, Warning, TEXT("PreLogin rejected: lobby full"));
		return;
	}

	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

	if (!ErrorMessage.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("PreLogin failed: %s"), *ErrorMessage);
	}

	// ParseOption ?쇰줈 ?먮룞 ?붿퐫??+ & 遺꾨━
	FString RawStage = UGameplayStatics::ParseOption(Options, TEXT("SelectedStage"));
	FString RawLobby = UGameplayStatics::ParseOption(Options, TEXT("LobbyName"));

	FString Stage = FGenericPlatformHttp::UrlDecode(RawStage);
	FString Lobby = FGenericPlatformHttp::UrlDecode(RawLobby);

	UE_LOG(LogTemp, Log, TEXT("[PreLogin] Options -> Stage=%s, Lobby=%s"), *Stage, *Lobby);

	// GameState?????(Replicated property)
	if (APolyPalsState* GS = GetGameState<APolyPalsState>())
	{
		if (!Stage.IsEmpty())
			GS->SetSelectedStage(*Stage);

		if (!Lobby.IsEmpty())
			GS->SetLobbyName(Lobby);
	}
}

void APolyPalsGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	const FString CurrentMap = UGameplayStatics::GetCurrentLevelName(GetWorld(), true);

	UE_LOG(LogTemp, Log, TEXT("[PostLogin] Player=%s, ConnectedPlayers=%d, Map=%s"), *NewPlayer->PlayerState->GetPlayerName(), ConnectedPlayers + 1, *CurrentMap);

	if (CurrentMap.Equals(TEXT("EmptyLevel"), ESearchCase::IgnoreCase))
	{
		if (!NewPlayer->IsLocalController())
		{
			FString ConnectedName = NewPlayer->PlayerState
				? NewPlayer->PlayerState->GetPlayerName()
				: TEXT("Unknown");

			UE_LOG(LogTemp, Log, TEXT("[Server] Player connected: %s"), *ConnectedName);

			if (!PreparedColors.IsEmpty())
			{
				if (APolyPalsController* ProjectController = Cast<APolyPalsController>(NewPlayer))
				{
					ProjectController->InitializeControllerDataByGameMode(PreparedColors[0]);
					PreparedColors.RemoveAt(0);
				}
			}
		}

		if (APolyPalsPlayerState* PS = Cast<APolyPalsPlayerState>(NewPlayer->PlayerState))
		{
			PS->SetSlotIndex(ConnectedPlayers);
			UE_LOG(LogTemp, Log, TEXT("Assigned SlotIndex = %d"), ConnectedPlayers);
		}

		ConnectedPlayers++;

		UE_LOG(LogTemp, Log, TEXT("Player connected: %d/%d"), ConnectedPlayers, ExpectedPlayerCount);

		if (ConnectedPlayers == 1)
		{
			if (APolyPalsState* GS = GetGameState<APolyPalsState>())
			{
				GS->OnAllPlayersReady.AddDynamic(this, &APolyPalsGameMode::HandleAllPlayersReady);
				GS->OnGameOver.AddDynamic(this, &APolyPalsGameMode::HandleStateGameOver);
			}
		}

		if (APolyPalsState* GS = GetGameState<APolyPalsState>())
		{
			GS->UpdateConnectedPlayers(ConnectedPlayers);
		}
	}
	else
	{
		if (HasAuthority() && GetWorld())
		{
			UE_LOG(LogTemp, Log, TEXT("GameMode:DistributeStartingGold()"));

			DistributeStartingGold();

			if (!PreparedColors.IsEmpty())
			{
				if (APolyPalsController* ProjectController = Cast<APolyPalsController>(NewPlayer))
				{
					ProjectController->InitializeControllerDataByGameMode(PreparedColors[0]);
					PreparedColors.RemoveAt(0);
				}
			}
		}
	}
}

void APolyPalsGameMode::StartPlay()
{
	Super::StartPlay();
}

void APolyPalsGameMode::TriggerGameOver()
{
	AGameStateBase* GS = GameState;

	if (APolyPalsState* PState = Cast<APolyPalsState>(GS))
	{
		PState->SetGameOver();
	}
}

void APolyPalsGameMode::ConfigureLobby(FName StageName, const FString& LobbyName)
{
	if (APolyPalsState* GS = GetGameState<APolyPalsState>())
	{
		GS->SetSelectedStage(StageName);
		GS->SetLobbyName(LobbyName);
	}
}

void APolyPalsGameMode::DistributeStartingGold()
{
	if (APolyPalsState* GS = GetGameState<APolyPalsState>())
	{
		// 현재 접속된 플레이어 수
		int32 PlayerCount = GS->PlayerArray.Num();

		// 플레이어 수 기준으로 골드 계산
		int32 StartingGold = CalculateStartingGold(PlayerCount);

		UE_LOG(LogTemp, Warning, TEXT("[DistributeStartingGold] PlayerCount=%d, StartingGold=%d"), PlayerCount, StartingGold);

		// 모든 플레이어에게 동일하게 설정
		for (APlayerState* PS : GS->PlayerArray)
		{
			if (APolyPalsPlayerState* PPS = Cast<APolyPalsPlayerState>(PS))
			{
				PPS->SetInitialGold(StartingGold);
				UE_LOG(LogTemp, Warning, TEXT(" → To %s: Gold=%d"), *PPS->GetPlayerName(), StartingGold);
			}
		}
	}
}

void APolyPalsGameMode::HandleAllPlayersReady()
{
	if (APolyPalsState* GS = GetGameState<APolyPalsState>())
	{
		GS->StartLobbyCountdown(static_cast<int32>(StartCountdownTime));
	}

	GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
	{
		StartGameAfterCountdown();
	});
}

void APolyPalsGameMode::StartGameAfterCountdown()
{
	FTimerHandle TimerHandle;

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
	{
		APolyPalsState* GS = GetGameState<APolyPalsState>();

		if (GS)
		{
			FName StageKey = GS->GetSelectedStage();
			FString MapName;

			if (StageMapPaths.Contains(StageKey))
			{
				MapName = StageMapPaths[StageKey];
			}
			else
			{
				MapName = StageKey.ToString();
			}

			FString TravelURL = FString::Printf(TEXT("/Game/Maps/%s"), *MapName);

			UE_LOG(LogTemp, Log, TEXT("[GameMode] ServerTravel to %s"), *TravelURL);

			GetWorld()->ServerTravel(TravelURL);
		}

	}, StartCountdownTime, false);
}

void APolyPalsGameMode::HandleStateGameOver()
{
	UE_LOG(LogTemp, Log, TEXT("[GameMode] GameOver"));
}

void APolyPalsGameMode::OnEnemyKilled(int32 InGold)
{
	DecreaseRemainingEnemyCount();
}

int32 APolyPalsGameMode::CalculateStartingGold(int32 PlayerCount) const
{
	int32 Calculated = StartingGoldMax - (PlayerCount - 1) * StartingGoldStep;

	return FMath::Clamp(Calculated, StartingGoldMin, StartingGoldMax);
}