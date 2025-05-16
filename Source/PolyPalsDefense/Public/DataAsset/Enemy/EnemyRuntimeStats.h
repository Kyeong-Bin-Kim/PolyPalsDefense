#pragma once

#include "CoreMinimal.h"
#include "EnemyStatData.h"
#include "EnemyRuntimeStats.generated.h"

USTRUCT(BlueprintType)
struct FEnemyRuntimeStats
{
    GENERATED_BODY()

    float MaxHealth;
    float CurrentHealth;
    float MoveSpeed;
    float Defence;

    FEnemyRuntimeStats() = default;

    FEnemyRuntimeStats(const FEnemyStatData& Base)
    {
        MaxHealth = Base.MaxHealth;
        CurrentHealth = Base.MaxHealth;
        MoveSpeed = Base.BaseMoveSpeed;
        Defence = Base.Defence;
    }

    void ApplyMultiplier(float HealthMultiplier, float SpeedMultiplier)
    {
        MaxHealth *= HealthMultiplier;
        CurrentHealth = MaxHealth;
        MoveSpeed *= SpeedMultiplier;
    }
};
