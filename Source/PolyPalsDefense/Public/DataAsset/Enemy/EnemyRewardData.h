#pragma once

#include "CoreMinimal.h"
#include "EnemyRewardData.generated.h"

USTRUCT(BlueprintType)
struct FEnemyRewardData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Reward")
    int32 Gold = 10;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Reward")
    int32 DamageToPlayer = 1;
};
