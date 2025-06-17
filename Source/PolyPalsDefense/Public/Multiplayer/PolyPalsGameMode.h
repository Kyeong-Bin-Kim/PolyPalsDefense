#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Tower/TowerEnums.h"
#include "PolyPalsGameMode.generated.h"

// 생명 변화 이벤트 델리게이트
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLifeChanged, float /*InCurrentLifeRatio*/);
// 라운드 변화 이벤트 델리게이트
DECLARE_MULTICAST_DELEGATE_OneParam(FOnRoundChanged, int32 /*AssignedColor*/);

class APolyPalsState;

UCLASS()
class POLYPALSDEFENSE_API APolyPalsGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APolyPalsGameMode();

protected:
	// 메인 UI 위젯 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> MainUIWidgetClass;

	// 메인 UI 위젯 인스턴스
	UPROPERTY()
	UUserWidget* MainUIWidgetInstance;

public:
	// GameState를 통해 게임 오버를 트리거
	UFUNCTION(BlueprintCallable, Category = "Game")
	void TriggerGameOver();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void StartPlay() override;

	UFUNCTION()
	void OnEnemyKilled(int32 InGold);

	// 생명 감소 함수
	inline void SubtractLife(int32 InValue)
	{
		SetLife(Life - InValue);
		if (bIsGamePlay && Life < 0)
		{
			Life = 0;
			// GameOver(); → 여기 대신 TriggerGameOver() 사용 가능
		}
	}

private:
	// 남은 적 수 감소
	inline void DecreaseRemainingEnemyCount()
	{
		RemainingEnemyCount--;
		if (bIsGamePlay && RemainingEnemyCount <= 0)
		{
			RemainingEnemyCount = 0;
			// GameClear(); → 필요 시 확장
		}
	}

	// 현재 생명 설정 및 생명 변화 이벤트 브로드캐스트
	inline void SetLife(int32 InNewLife)
	{
		Life = InNewLife;
		OnHealthChanged.Broadcast(static_cast<float>(Life) / static_cast<float>(StartLife)); // HealthBar에 현재 체력 비율 전달
	}

public:
	// 생명 변화 이벤트 (UI 연동용)
	FOnLifeChanged OnHealthChanged;
	// 라운드 변화 이벤트 (UI 연동용)
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

	// 게임 시작 후 플레이 상태 플래그
	bool bIsGamePlay = true;

	// 스테이지 시작 체력
	int32 StartLife = 50;

	// 준비 완료 후 대기 시간 (초)
	UPROPERTY(EditDefaultsOnly, Category = "Game")
	float StartCountdownTime = 5.0f;

	// 플레이어 시작 골드 계산용 최대/최소/감소 단위
	UPROPERTY(EditDefaultsOnly, Category = "Economy")
	int32 StartingGoldMax = 2000;

	UPROPERTY(EditDefaultsOnly, Category = "Economy")
	int32 StartingGoldMin = 500;

	// 플레이어 한 명이 추가될 때마다 감소할 골드
	UPROPERTY(EditDefaultsOnly, Category = "Economy")
	int32 StartingGoldStep = 500;

	// 스테이지 이름과 로드할 맵 이름 매핑
	UPROPERTY(EditDefaultsOnly, Category = "Stage")
	TMap<FName, FString> StageMapPaths;

protected:
	// GameState의 AllPlayersReady 이벤트 처리
	UFUNCTION()
	void HandleAllPlayersReady();

	// 플레이어 수에 따른 시작 골드 계산
	int32 CalculateStartingGold(int32 PlayerCount) const;

	// GameState의 GameOver 이벤트 처리
	UFUNCTION()
	void HandleStateGameOver();

	// 준비 완료 후 카운트다운 후 게임 시작
	void StartGameAfterCountdown();
};
