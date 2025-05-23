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

    UPROPERTY(EditAnywhere, Category = "Enemy|Visual")
    FVector Scale = FVector(0.25f, 0.25f, 0.25f);

    UPROPERTY(EditAnywhere, Category = "Enemy|Stats")
    float HealthMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, Category = "Enemy|Stats")
    float SpeedMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, Category = "Enemy|Collision")
    float CapsuleRadius = 34.f;

    // 구체 콜리전 기본 반지름
    UPROPERTY(EditAnywhere, Category = "Enemy|Collision")
    float BasicSphereRadius = 60.f;
};
