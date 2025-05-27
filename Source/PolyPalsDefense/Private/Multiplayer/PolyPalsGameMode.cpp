#include "PolyPalsGameMode.h"
#include "PolyPalsState.h"
#include "PolyPalsController.h"
#include "Tower/TestEnemy.h"

APolyPalsGameMode::APolyPalsGameMode()
{
	ConnectedPlayers = 0;
	ExpectedPlayerCount = 2;
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

	if (!NewPlayer->IsLocalController())
	{
		UE_LOG(LogTemp, Log, TEXT("Client entered"));

		if (!PreparedColors.IsEmpty())
		{
			APolyPalsController* ProjectController = Cast<APolyPalsController>(NewPlayer);
			ProjectController->InitializeControllerDataByGameMode(PreparedColors[0]);
			PreparedColors.RemoveAt(0);
		}
	}

	// 접속자 수 카운트 (호스트 포함)
	ConnectedPlayers++;
	UE_LOG(LogTemp, Log, TEXT("Player connected: %d/%d"),
		ConnectedPlayers, ExpectedPlayerCount);

	// 기준 인원 도달 시 OnAllPlayersReady 브로드캐스트
	if (ConnectedPlayers >= ExpectedPlayerCount)
	{
		if (APolyPalsState* GS = GetGameState<APolyPalsState>())
		{
			UE_LOG(LogTemp, Log, TEXT("All players ready! Broadcasting event."));
			GS->OnAllPlayersReady.Broadcast();
		}
	}
}

void APolyPalsGameMode::StartPlay()
{
	Super::StartPlay();

	/////////////////////// test code ///////////////////////
	//FActorSpawnParameters Params;
	//Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	//FTransform SpawnTrasform;
	//SpawnTrasform.SetLocation(TestEnemySpawnLocation);

	//GetWorld()->SpawnActor<ATestEnemy>(TestEnemyClass, SpawnTrasform);
	//////////////////////////////////////////////////////////
}

void APolyPalsGameMode::OnEnemyKilled(int32 InGold)
{
	DecreaseRemainingEnemyCount(); // 남은 적 수 감소
}
