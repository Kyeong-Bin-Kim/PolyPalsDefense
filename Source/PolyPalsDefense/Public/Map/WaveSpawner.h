#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Map/EnemySpawnEntry.h"
#include "Map/EnemySpawnPlan.h"
#include "WaveSpawner.generated.h"

class UEnemyPoolComponent;
class USplineComponent;

UCLASS()
class POLYPALSDEFENSE_API AWaveSpawner : public AActor
{
    GENERATED_BODY()

public:
    AWaveSpawner();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    void StartWave(int32 RoundIndex);

protected:
    UPROPERTY(EditAnywhere, Category = "Wave")
    TArray<FEnemySpawnPlan> Rounds;

    UPROPERTY(EditAnywhere, Category = "Wave")
    USplineComponent* SplinePath;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UEnemyPoolComponent* EnemyPool;

private:
    int32 SpawnIndex = 0;
    TArray<FPrimaryAssetId> CurrentSpawnList;
    FTimerHandle SpawnTimerHandle;

    void SpawnNextEnemy();
};

