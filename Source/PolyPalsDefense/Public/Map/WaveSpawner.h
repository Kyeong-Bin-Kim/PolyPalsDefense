#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Map/EnemySpawnEntry.h"
#include "WaveSpawner.generated.h"

class UEnemyPoolComponent;
class USplineComponent;
class AStageActor;

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
    AStageActor* StageRef;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UEnemyPoolComponent* EnemyPool;

private:
    USplineComponent* SplinePath;

    int32 SpawnIndex = 0;
    TArray<FEnemySpawnEntry> CurrentSpawnList;
    FTimerHandle SpawnTimerHandle;

    void SpawnNextEnemy();
};

