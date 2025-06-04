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

    // 게임 오버 이벤트 핸들러
    UFUNCTION()
    void HandleGameOver();


public:
    // 라운드를 시작하는 함수
    UFUNCTION(BlueprintCallable, Category = "Wave")
    void StartWave(int32 RoundIndex);

    UFUNCTION()
    void OnEnemyKilled();


    int32 TotalEnemiesThisWave = 0;
    int32 RemainingEnemies = 0;
protected:
    // 에디터에서 지정하거나 자동 검색될 StageActor 참조
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
    AStageActor* StageRef;

    // 풀링 시스템 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UEnemyPoolComponent* EnemyPool;

private:
    // 스플라인 경로 및 DataTable
    USplineComponent* SplinePath;
    UDataTable* WavePlanTable;

    // 스폰 로직 관련
    TArray<FEnemySpawnEntry> CurrentSpawnList;
    int32 SpawnIndex;
    FTimerHandle SpawnTimerHandle;

    // SpawnNextEnemy 호출
    void SpawnNextEnemy();

};