#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Tower/TowerEnums.h"
#include "PolyPalsGameMode.generated.h"

// ?앸챸 蹂???대깽???몃━寃뚯씠??
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLifeChanged, float /*InCurrentLifeRatio*/);
// ?쇱슫??蹂???대깽???몃━寃뚯씠??
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
	// GameState瑜??듯빐 寃뚯엫 ?ㅻ쾭瑜??몃━嫄?
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

	// ?앸챸 媛먯냼 ?⑥닔
	inline void SubtractLife(int32 InValue)
	{
		SetLife(Life - InValue);
		if (bIsGamePlay && Life < 0)
		{
			Life = 0;
			// GameOver(); ???ш린 ???TriggerGameOver() ?ъ슜 媛??
		}
	}

private:
	// ?⑥? ????媛먯냼
	inline void DecreaseRemainingEnemyCount()
	{
		RemainingEnemyCount--;
		if (bIsGamePlay && RemainingEnemyCount <= 0)
		{
			RemainingEnemyCount = 0;
			// GameClear(); ???꾩슂 ???뺤옣
		}
	}

	// ?꾩옱 ?앸챸 ?ㅼ젙 諛??앸챸 蹂???대깽??釉뚮줈?쒖틦?ㅽ듃
	inline void SetLife(int32 InNewLife)
	{
		Life = InNewLife;
		OnHealthChanged.Broadcast(static_cast<float>(Life) / static_cast<float>(StartLife)); // HealthBar???꾩옱 泥대젰 鍮꾩쑉 ?꾨떖
	}

public:
	// ?앸챸 蹂???대깽??(UI ?곕룞??
	FOnLifeChanged OnHealthChanged;
	// ?쇱슫??蹂???대깽??(UI ?곕룞??
	FOnRoundChanged OnRoundChanged;

protected:
	UPROPERTY()
	int32 MaxPlayerSlots = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StageData")
	int32 Life = 100;

	// ???ㅽ뀒?댁??먯꽌 ?⑥븘?덈뒗 ?곸쓽 ??
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StageData")
	int32 RemainingEnemyCount = 0;

private:
	UPROPERTY()
	TArray<EPlayerColor> PreparedColors;

	// ?꾩옱 ?묒냽???뚮젅?댁뼱 ??
	int32 ConnectedPlayers;

	// ?쒖옉???꾩슂??理쒖냼 ?뚮젅?댁뼱 ??(?먮뵒?곗뿉?쒕룄 議곗젙 媛??
	UPROPERTY(EditDefaultsOnly, Category = "Game")
	int32 ExpectedPlayerCount;

	// 寃뚯엫 ?쒖옉 ???뚮젅???곹깭 ?뚮옒洹?
	bool bIsGamePlay = true;

	// ?ㅽ뀒?댁? ?쒖옉 泥대젰
	int32 StartLife = 50;

	// 以鍮??꾨즺 ???湲??쒓컙 (珥?
	UPROPERTY(EditDefaultsOnly, Category = "Game")
	float StartCountdownTime = 5.0f;

	// ?뚮젅?댁뼱 ?쒖옉 怨⑤뱶 怨꾩궛??理쒕?/理쒖냼/媛먯냼 ?⑥쐞
	UPROPERTY(EditDefaultsOnly, Category = "Economy")
	int32 StartingGoldMax = 2000;

	UPROPERTY(EditDefaultsOnly, Category = "Economy")
	int32 StartingGoldMin = 500;

	// ?뚮젅?댁뼱 ??紐낆씠 異붽????뚮쭏??媛먯냼??怨⑤뱶
	UPROPERTY(EditDefaultsOnly, Category = "Economy")
	int32 StartingGoldStep = 500;

	// ?ㅽ뀒?댁? ?대쫫怨?濡쒕뱶??留??대쫫 留ㅽ븨
	UPROPERTY(EditDefaultsOnly, Category = "Stage")
	TMap<FName, FString> StageMapPaths;

protected:
	// 紐⑤뱺 ?뚮젅?댁뼱?먭쾶 ?쒖옉 怨⑤뱶 吏湲?
	void DistributeStartingGold();

	// GameState??AllPlayersReady ?대깽??泥섎━
	UFUNCTION()
	void HandleAllPlayersReady();

	// ?뚮젅?댁뼱 ?섏뿉 ?곕Ⅸ ?쒖옉 怨⑤뱶 怨꾩궛
	int32 CalculateStartingGold(int32 PlayerCount) const;

	// GameState??GameOver ?대깽??泥섎━
	UFUNCTION()
	void HandleStateGameOver();

	// 以鍮??꾨즺 ??移댁슫?몃떎????寃뚯엫 ?쒖옉
	void StartGameAfterCountdown();
};
