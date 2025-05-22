#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "EnemySpawnEntry.h"
#include "WavePlanRow.generated.h"

USTRUCT(BlueprintType)
struct FWavePlanRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 RoundIndex;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
    float SpawnWindow = 40.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FEnemySpawnEntry> SpawnList;
};