#pragma once

#include "CoreMinimal.h"
#include "EnemySpawnEntry.generated.h"

USTRUCT(BlueprintType)
struct FEnemySpawnEntry
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    FPrimaryAssetId EnemyId;

    UPROPERTY(EditAnywhere)
    int32 Count = 10;
};
