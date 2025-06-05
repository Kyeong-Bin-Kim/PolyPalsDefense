#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PolyPalsState.h"
#include "WaveManager.generated.h"

class AWaveSpawner;
class AEnemyPawn;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWaveInfoChanged);

UCLASS()
class POLYPALSDEFENSE_API AWaveManager : public AActor
{
    GENERATED_BODY()

public:
    AWaveManager();

    UPROPERTY(BlueprintAssignable, Category = "Event")
    FOnWaveInfoChanged OnWaveInfoChanged;

    UFUNCTION()
    void NotifyWaveInfoChanged();

    // 리플리케이션 설정
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnPlayersReady();

    UFUNCTION()
    void HandleGameOver();

    // 리플리케이션 동기화 콜백
    UFUNCTION()
    void OnRep_PlayerLife();

    UFUNCTION()
    void OnRep_CurrentRound();


public:
    // === UI용 Getter ===
    UFUNCTION(BlueprintPure, Category = "Wave|UI")
    int32 GetRemainingLives() const { return PlayerLife; }

    UFUNCTION(BlueprintPure, Category = "Wave|UI")
    int32 GetCurrentRoundIndex() const { return CurrentRoundIndex; }

    UFUNCTION(BlueprintPure, Category = "Wave|UI")
    int32 GetTotalRoundCount() const { return TotalRoundCount; }

    int32 GetTotalEnemiesThisWave() const;
    int32 GetRemainingEnemiesThisWave() const;

    UFUNCTION(BlueprintPure, Category = "Wave|UI")
    float GetRoundElapsedTime() const;

    UFUNCTION(BlueprintPure, Category = "Wave|UI")
    float GetPreparationTime() const { return PreparationTime; }

    UFUNCTION(BlueprintPure, Category = "Wave|UI")
	float GetRoundDuration() const { return RoundDuration; }


    UFUNCTION(BlueprintPure, Category = "Wave|UI")
    int32 GetEnemiesRemaining() const { return EnemiesLeft; }

    UFUNCTION(BlueprintCallable, Category = "Wave")
    void StartRound(int32 RoundIndex);

    UFUNCTION()
    void HandleEnemyReachedGoal(AEnemyPawn* Enemy);

    UFUNCTION()
    bool IsInPreparationPhase() const { return bIsPreparingPhase; }

private:
    UFUNCTION()
    void StartFirstRound();

    void EndRound();

    UPROPERTY(ReplicatedUsing = OnRep_CurrentRound, VisibleAnywhere, Category = "Wave")
    int32 CurrentRoundIndex = 1;

    int32 TotalRoundCount = 10;
    FTimerHandle RoundTimerHandle;
    float RoundStartTimestamp = 0.f;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
    AWaveSpawner* WaveSpawner;

    UPROPERTY(EditAnywhere, Category = "Wave")
    float PreparationTime = 30.f;

    UPROPERTY(EditAnywhere, Category = "Wave")
    float RoundDuration = 60.f;

    // 클라이언트 동기화를 위한 리플리케이션 설정
    UPROPERTY(ReplicatedUsing = OnRep_PlayerLife, VisibleAnywhere, Category = "Game")
    int32 PlayerLife = 50;

    UPROPERTY(EditAnywhere, Category = "Game")
    int32 BasicDamageToLife = 1;

    UPROPERTY(EditAnywhere, Category = "Game")
    int32 BossDamageToLife = 10;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
    int32 EnemiesLeft = 0;

private:
    bool bIsPreparingPhase = true;
};
