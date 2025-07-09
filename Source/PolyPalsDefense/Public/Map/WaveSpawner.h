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

    // 寃뚯엫 ?ㅻ쾭 ?대깽???몃뱾??
    UFUNCTION()
    void HandleGameOver();


public:
    // ?쇱슫?쒕? ?쒖옉?섎뒗 ?⑥닔
    UFUNCTION(BlueprintCallable, Category = "Wave")
    void StartWave(int32 RoundIndex);

    UFUNCTION()
    void OnEnemyKilled();


    int32 TotalEnemiesThisWave = 0;
    int32 RemainingEnemies = 0;
protected:
    // ?먮뵒?곗뿉??吏?뺥븯嫄곕굹 ?먮룞 寃?됰맆 StageActor 李몄“
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
    AStageActor* StageRef;

    // ?留??쒖뒪??而댄룷?뚰듃
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UEnemyPoolComponent* EnemyPool;

private:
    // ?ㅽ뵆?쇱씤 寃쎈줈 諛?DataTable
    USplineComponent* SplinePath;
    UDataTable* WavePlanTable;

    // ?ㅽ룿 濡쒖쭅 愿??
    TArray<FEnemySpawnEntry> CurrentSpawnList;
    int32 SpawnIndex;
    FTimerHandle SpawnTimerHandle;

    // SpawnNextEnemy ?몄텧
    void SpawnNextEnemy();

};