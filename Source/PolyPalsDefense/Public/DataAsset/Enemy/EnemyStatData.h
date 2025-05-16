#pragma once

#include "CoreMinimal.h"
#include "EnemyStatData.generated.h"

USTRUCT(BlueprintType)
struct FEnemyStatData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Stats")
    float MaxHealth = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Stats")
    float BaseMoveSpeed = 300.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Stats")
    float Defence = 0.f;
};
