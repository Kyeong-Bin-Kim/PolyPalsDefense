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

public:
	// GameState를 통해 게임 오버를 트리거
	UFUNCTION(BlueprintCallable, Category = "Game")
	void TriggerGameOver();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void StartPlay() override;

	UFUNCTION()
	void OnEnemyKilled(int32 InGold);
	

	inline void SubtractLife(int32 InValue)
	{
		SetLife(Life - InValue);
		if (bIsGamePlay && Life < 0)
		{
			Life = 0;
			//GameOver();
		}
	}
protected:
	///////////////////// test code ///////////////////////
	//UPROPERTY(EditDefaultsOnly)
	//TSubclassOf<class ATestEnemy> TestEnemyClass;
	//UPROPERTY(EditDefaultsOnly)
	//FVector TestEnemySpawnLocation;
	////////////////////////////////////////////////////////

private:

	inline void DecreaseRemainingEnemyCount()
	{
		RemainingEnemyCount--;
		if (bIsGamePlay && RemainingEnemyCount <= 0)
		{
			RemainingEnemyCount = 0;
			//GameClear();
		}
	}
	inline void SetLife(int32 InNewHealth)
	{
		Life = InNewHealth;
		OnHealthChanged.Broadcast(static_cast<float>(Life) / static_cast<float>(StartHealth)); // HealthBar에 현재 체력 비율 전달
	}

public:

	FOnLifeChanged OnHealthChanged;
	FOnRoundChanged OnRoundChanged;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StageData")
	int32 Life = 100;

	// 이 스테이지에서 남아있는 적의 수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StageData")
	int32 RemainingEnemyCount = 0;


private:
	UPROPERTY()
	TArray<EPlayerColor> PreparedColors;

	// 현재 접속된 플레이어 수
	int32 ConnectedPlayers;

	// 시작에 필요한 최소 플레이어 수 (에디터에서도 조정 가능)
	UPROPERTY(EditDefaultsOnly, Category = "Game")
	int32 ExpectedPlayerCount;


	bool bIsGamePlay = true;
	int32 StartHealth = 100;
};