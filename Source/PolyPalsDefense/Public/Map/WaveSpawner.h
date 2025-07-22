#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Map/EnemySpawnEntry.h"
#include "PolyPalsState.h"
#include "WaveSpawner.generated.h"

class UEnemyPoolComponent;
class USplineComponent;
class UDataTable;
class AStageActor;
class AEnemyPawn;

UCLASS()
class POLYPALSDEFENSE_API AWaveSpawner : public AActor
{
    GENERATED_BODY()

public:
    AWaveSpawner();

protected:
    virtual void BeginPlay() override;

    // 野껊슣????살쒔 ??源???紐껊굶??
    UFUNCTION()
    void HandleGameOver();


public:
    // ??깆뒲??? ??뽰삂??롫뮉 ??λ땾
    UFUNCTION(BlueprintCallable, Category = "Wave")
    void StartWave(int32 RoundIndex);

    UFUNCTION()
    void OnEnemyKilled();


    int32 TotalEnemiesThisWave = 0;
    int32 RemainingEnemies = 0;
protected:
    // ?癒?탵?怨쀫퓠??筌왖?類λ릭椰꾧퀡援??癒?짗 野꺜??곕쭍 StageActor 筌〓챷??
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
    AStageActor* StageRef;

    // ??筌???뽯뮞???뚮똾猷??곕뱜
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UEnemyPoolComponent* EnemyPool;

private:
    // ??쎈탣??깆뵥 野껋럥以?獄?DataTable
    USplineComponent* SplinePath;
    UDataTable* WavePlanTable;

    // ??쎈？ 嚥≪뮇彛??온??
    TArray<FEnemySpawnEntry> CurrentSpawnList;
    int32 SpawnIndex;
    FTimerHandle SpawnTimerHandle;

    // SpawnNextEnemy ?紐꾪뀱
    void SpawnNextEnemy();

};