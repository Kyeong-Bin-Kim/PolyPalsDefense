#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Tower/TowerEnums.h"
#include "PolyPalsGameMode.generated.h"

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

private:
	UPROPERTY()
	TArray<EPlayerColor> PreparedColors;

	// 현재 접속된 플레이어 수
	int32 ConnectedPlayers;

	// 시작에 필요한 최소 플레이어 수 (에디터에서도 조정 가능)
	UPROPERTY(EditDefaultsOnly, Category = "Game")
	int32 ExpectedPlayerCount;

};