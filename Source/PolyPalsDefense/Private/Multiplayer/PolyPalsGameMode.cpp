#include "PolyPalsGameMode.h"
#include "PolyPalsState.h"
#include "PolyPalsPlayerState.h"
#include "PolyPalsController.h"
#include "PolyPalsGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

APolyPalsGameMode::APolyPalsGameMode()
{
	ConnectedPlayers = 0;

	// 湲곕낯 ?ㅽ뀒?댁?-留?留ㅽ븨 ?ㅼ젙
	StageMapPaths.Add(TEXT("Dirt"), TEXT("DirtStage"));
	StageMapPaths.Add(TEXT("Snow"), TEXT("SnowStage"));
}

void APolyPalsGameMode::BeginPlay()
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

void APolyPalsGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (!NewPlayer->IsLocalController()) // ?대씪?댁뼵?몃쭔 泥섎━
	{
		UE_LOG(LogTemp, Log, TEXT("Client entered"));

		if (!PreparedColors.IsEmpty())
		{
			if (APolyPalsController* ProjectController = Cast<APolyPalsController>(NewPlayer))
			{
				ProjectController->InitializeControllerDataByGameMode(PreparedColors[0]);
				PreparedColors.RemoveAt(0);

				// 1) ?꾩옱 GameState 媛?몄삤湲?
				APolyPalsState* GS = GetGameState<APolyPalsState>();

				// 2) PlayerState ?ъ씤??罹먯떆
				APlayerState* PS = NewPlayer->PlayerState;

				// 3) GameState? PlayerState媛 紐⑤몢 ?좏슚???뚮쭔 吏꾪뻾
				FString HostName;

				if (GS)
				{
					if (ConnectedPlayers == 0)  // 泥?踰덉㎏ ?묒냽?먮㈃ ?몄뒪??
					{
						HostName = NewPlayer->PlayerState->GetPlayerName();
						GS->SetLobbyName(HostName);
					}
					else
					{
						HostName = GS->GetLobbyName();
					}

					// 4) ?대씪?댁뼵?몄뿉 RPC ?몄텧 (StageName/LobbyName ?ы븿)
					ProjectController->Client_ShowLobbyUI(HostName, GS->GetSelectedStage(), GS->GetLobbyName());
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("PostLogin: GameState or PlayerState is null (GS=%p, PS=%p)"), GS, PS);
				}
			}
		}
	}

	// 1. SlotIndex 遺??
	if (APolyPalsPlayerState* PS = Cast<APolyPalsPlayerState>(NewPlayer->PlayerState))
	{
		PS->SetSlotIndex(ConnectedPlayers); // GameState???깅줉?섍린 ??ConnectedPlayers 湲곗?
		UE_LOG(LogTemp, Log, TEXT("Assigned SlotIndex = %d"), ConnectedPlayers);
	}

	// 2. ?묒냽????利앷?
	ConnectedPlayers++;

	UE_LOG(LogTemp, Log, TEXT("Player connected: %d/%d"), ConnectedPlayers, ExpectedPlayerCount);

	// 3. GameState 諛붿씤??(泥??뚮젅?댁뼱???뚮쭔)
	if (ConnectedPlayers == 1)
	{
		if (APolyPalsState* GS = GetGameState<APolyPalsState>())
		{
			GS->OnAllPlayersReady.AddDynamic(this, &APolyPalsGameMode::HandleAllPlayersReady);
			GS->OnGameOver.AddDynamic(this, &APolyPalsGameMode::HandleStateGameOver);
		}
	}

	// 4. GameState???묒냽????諛섏쁺
	if (APolyPalsState* GS = GetGameState<APolyPalsState>())
	{
		GS->UpdateConnectedPlayers(ConnectedPlayers);
	}
}

void APolyPalsGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	UE_LOG(LogTemp, Log, TEXT("PreLogin from %s"), *Address);

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
}

void APolyPalsGameMode::StartPlay()
{
	Super::StartPlay();

	// 寃뚯엫 ?쒖옉 ??紐⑤뱺 ?뚮젅?댁뼱?먭쾶 珥덇린 怨⑤뱶 吏湲?
	DistributeStartingGold();
}

void APolyPalsGameMode::TriggerGameOver()
{
	// GameState瑜?PolyPalsState濡?罹먯뒪?명븯??SetGameOver ?몄텧
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
		int32 PlayerCount = GS->PlayerArray.Num();
		int32 StartingGold = CalculateStartingGold(PlayerCount);
		for (APlayerState* PS : GS->PlayerArray)
		{
			if (APolyPalsPlayerState* PPS = Cast<APolyPalsPlayerState>(PS))
			{
				PPS->SetInitialGold(StartingGold);
			}
		}
	}
}

void APolyPalsGameMode::HandleAllPlayersReady()
{
	UE_LOG(LogTemp, Log, TEXT("[GameMode] 紐⑤뱺 ?뚮젅?댁뼱 以鍮??꾨즺! %f珥???寃뚯엫 ?쒖옉"), StartCountdownTime);

	// 移댁슫?몃떎????寃뚯엫 ?쒖옉
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
		//留듭쑝濡??대룞?섏뿬 寃뚯엫 ?쒖옉
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

			FString TravelURL = FString::Printf(TEXT("/Game/%s?listen"), *MapName);
			GetWorld()->ServerTravel(TravelURL);
		}

	}, StartCountdownTime, false);
}

void APolyPalsGameMode::HandleStateGameOver()
{
	UE_LOG(LogTemp, Warning, TEXT("[GameMode] 寃뚯엫 ?ㅻ쾭 泥섎━ ?쒖옉"));

	// 鍮?留듭쑝濡?蹂듦?
	GetWorld()->ServerTravel(TEXT("/Game/EmptyLevel?listen"));
}

void APolyPalsGameMode::OnEnemyKilled(int32 InGold)
{
	DecreaseRemainingEnemyCount(); // ?⑥? ????媛먯냼
}

int32 APolyPalsGameMode::CalculateStartingGold(int32 PlayerCount) const
{
	int32 Calculated = StartingGoldMax - (PlayerCount - 1) * StartingGoldStep;

	return FMath::Clamp(Calculated, StartingGoldMin, StartingGoldMax);
}