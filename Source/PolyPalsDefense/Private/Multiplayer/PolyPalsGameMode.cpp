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

	// 기본 스테이지-맵 매핑 설정
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

	if (!NewPlayer->IsLocalController()) // 클라이언트만 처리
	{
		UE_LOG(LogTemp, Log, TEXT("Client entered"));

		if (!PreparedColors.IsEmpty())
		{
			if (APolyPalsController* ProjectController = Cast<APolyPalsController>(NewPlayer))
			{
				ProjectController->InitializeControllerDataByGameMode(PreparedColors[0]);
				PreparedColors.RemoveAt(0);

				// 1) 현재 GameState 가져오기
				APolyPalsState* GS = GetGameState<APolyPalsState>();

				// 2) PlayerState 포인터 캐시
				APlayerState* PS = NewPlayer->PlayerState;

				// 3) GameState와 PlayerState가 모두 유효할 때만 진행
				FString HostName;

				if (GS)
				{
					if (ConnectedPlayers == 0)  // 첫 번째 접속자면 호스트
					{
						HostName = NewPlayer->PlayerState->GetPlayerName();
						GS->SetLobbyName(HostName);
					}
					else
					{
						HostName = GS->GetLobbyName();
					}

					// 4) 클라이언트에 RPC 호출 (StageName/LobbyName 포함)
					ProjectController->Client_ShowLobbyUI(HostName, GS->GetSelectedStage(), GS->GetLobbyName());
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("PostLogin: GameState or PlayerState is null (GS=%p, PS=%p)"), GS, PS);
				}
			}
		}
	}

	// 1. SlotIndex 부여
	if (APolyPalsPlayerState* PS = Cast<APolyPalsPlayerState>(NewPlayer->PlayerState))
	{
		PS->SetSlotIndex(ConnectedPlayers); // GameState에 등록되기 전 ConnectedPlayers 기준
		UE_LOG(LogTemp, Log, TEXT("Assigned SlotIndex = %d"), ConnectedPlayers);
	}

	// 2. 접속자 수 증가
	ConnectedPlayers++;

	UE_LOG(LogTemp, Log, TEXT("Player connected: %d/%d"), ConnectedPlayers, ExpectedPlayerCount);

	// 3. GameState 바인딩 (첫 플레이어일 때만)
	if (ConnectedPlayers == 1)
	{
		if (APolyPalsState* GS = GetGameState<APolyPalsState>())
		{
			GS->OnAllPlayersReady.AddDynamic(this, &APolyPalsGameMode::HandleAllPlayersReady);
			GS->OnGameOver.AddDynamic(this, &APolyPalsGameMode::HandleStateGameOver);
		}
	}

	// 4. GameState에 접속자 수 반영
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

	// 초기 골드 지급
	FString CurrentMap = GetWorld()->GetMapName();

	if (!CurrentMap.Contains(TEXT("EmptyLevel")))
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
}

void APolyPalsGameMode::TriggerGameOver()
{
	// GameState를 PolyPalsState로 캐스트하여 SetGameOver 호출
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

void APolyPalsGameMode::HandleAllPlayersReady()
{
	UE_LOG(LogTemp, Log, TEXT("[GameMode] 모든 플레이어 준비 완료! %f초 뒤 게임 시작"), StartCountdownTime);

	// 카운트다운 후 게임 시작
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
		//맵으로 이동하여 게임 시작
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
	UE_LOG(LogTemp, Warning, TEXT("[GameMode] 게임 오버 처리 시작"));

	// 빈 맵으로 복귀
	GetWorld()->ServerTravel(TEXT("/Game/EmptyLevel?listen"));
}

void APolyPalsGameMode::OnEnemyKilled(int32 InGold)
{
	DecreaseRemainingEnemyCount(); // 남은 적 수 감소
}

int32 APolyPalsGameMode::CalculateStartingGold(int32 PlayerCount) const
{
	int32 Calculated = StartingGoldMax - (PlayerCount - 1) * StartingGoldStep;

	return FMath::Clamp(Calculated, StartingGoldMin, StartingGoldMax);
}