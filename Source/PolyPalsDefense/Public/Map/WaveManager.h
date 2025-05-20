#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaveManager.generated.h"

class AWaveSpawner;
class AEnemyPawn;

UCLASS()
class POLYPALSDEFENSE_API AWaveManager : public AActor
{
    GENERATED_BODY()

public:
    AWaveManager();

    virtual void BeginPlay() override;

    void StartNextRound();

    void HandleEnemyReachedGoal(AEnemyPawn* Enemy);

protected:
    UPROPERTY(EditAnywhere, Category = "Wave")
    TArray<AWaveSpawner*> Spawners;

    UPROPERTY(EditAnywhere, Category = "Wave")
    float RoundDuration = 60.f;

    UPROPERTY(EditAnywhere, Category = "Game")
    int32 PlayerLife = 50;

    UPROPERTY(EditAnywhere, Category = "Game")
    int32 BasicDamageToLife = 1;

    UPROPERTY(EditAnywhere, Category = "Game")
    int32 BossDamageToLife = 10;

private:
    int32 CurrentRoundIndex = 0;
    FTimerHandle RoundTimerHandle;

    void StartRound(int32 RoundIndex);
    void EndRound();
};
