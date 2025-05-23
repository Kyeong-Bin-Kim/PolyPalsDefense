#pragma once

#include "CoreMinimal.h"
#include "EnemySpawnEntry.generated.h"

USTRUCT(BlueprintType)
struct FEnemySpawnEntry
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, meta = (AllowedTypes = "EnemyDataAsset"))
    FPrimaryAssetId EnemyId;

    UPROPERTY(EditAnywhere)
    int32 Count = 1;

    UPROPERTY(EditAnywhere)
    bool bIsBoss = false;

    UPROPERTY(EditAnywhere)
    FVector Scale = FVector(0.25f, 0.25f, 0.25f);

    UPROPERTY(EditAnywhere)
    float HealthMultiplier = 1.0f;

    UPROPERTY(EditAnywhere)
    float SpeedMultiplier = 1.0f;
};
