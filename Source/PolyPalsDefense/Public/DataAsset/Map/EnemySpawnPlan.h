#pragma once

#include "CoreMinimal.h"
#include "Map/EnemySpawnEntry.h"
#include "EnemySpawnPlan.generated.h"

USTRUCT(BlueprintType)
struct FEnemySpawnPlan
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    int32 RoundIndex;

    UPROPERTY(EditAnywhere)
    float SpawnInterval = 0.5f;

    UPROPERTY(EditAnywhere)
    TArray<FEnemySpawnEntry> SpawnList;
};

