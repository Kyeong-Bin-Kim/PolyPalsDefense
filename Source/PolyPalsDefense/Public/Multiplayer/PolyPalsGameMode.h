#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Tower/TowerEnums.h"
#include "PolyPalsGameMode.generated.h"


DECLARE_MULTICAST_DELEGATE_OneParam(FOnLifeChanged, float /*InCurrentLifeRatio*/);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnRoundChanged, int32 /*AssignedColor*/);

class APolyPalsState;

UCLASS()
class POLYPALSDEFENSE_API APolyPalsGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APolyPalsGameMode();

	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Game")
	void TriggerGameOver();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void StartPlay() override;

	void SetMaxPlayerSlots(int32 InMax) { MaxPlayerSlots = InMax; }

	void ConfigureLobby(FName StageName, const FString& LobbyName);

	UFUNCTION()
	void OnEnemyKilled(int32 InGold);

	inline void SubtractLife(int32 InValue)
	{
		SetLife(Life - InValue);
		if (bIsGamePlay && Life < 0)
		{
			Life = 0;
		}
	}

private:
	inline void DecreaseRemainingEnemyCount()
	{
		RemainingEnemyCount--;
		if (bIsGamePlay && RemainingEnemyCount <= 0)
		{
			RemainingEnemyCount = 0;
		}
	}


	inline void SetLife(int32 InNewLife)
	{
		Life = InNewLife;
		OnHealthChanged.Broadcast(static_cast<float>(Life) / static_cast<float>(StartLife));
	}

public:
	FOnLifeChanged OnHealthChanged;

	FOnRoundChanged OnRoundChanged;

protected:
	UPROPERTY()
	int32 MaxPlayerSlots = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StageData")
	int32 Life = 100;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StageData")
	int32 RemainingEnemyCount = 0;

private:
	UPROPERTY()
	TArray<EPlayerColor> PreparedColors;

	int32 ConnectedPlayers;

	UPROPERTY(EditDefaultsOnly, Category = "Game")
	int32 ExpectedPlayerCount;

	bool bIsGamePlay = true;

	int32 StartLife = 50;

	UPROPERTY(EditDefaultsOnly, Category = "Game")
	float StartCountdownTime = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Economy")
	int32 StartingGoldMax = 2000;

	UPROPERTY(EditDefaultsOnly, Category = "Economy")
	int32 StartingGoldMin = 500;

	UPROPERTY(EditDefaultsOnly, Category = "Economy")
	int32 StartingGoldStep = 500;

	UPROPERTY(EditDefaultsOnly, Category = "Stage")
	TMap<FName, FString> StageMapPaths;

protected:
	void DistributeStartingGold();

	UFUNCTION()
	void HandleAllPlayersReady();

	int32 CalculateStartingGold(int32 PlayerCount) const;

	UFUNCTION()
	void HandleStateGameOver();

	void StartGameAfterCountdown();
};
