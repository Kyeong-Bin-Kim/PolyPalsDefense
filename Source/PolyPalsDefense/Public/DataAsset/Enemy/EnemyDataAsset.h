#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyStatData.h"
#include "Enemy/EnemyRewardData.h"
#include "Enemy/EnemyVisualData.h"
#include "EnemyDataAsset.generated.h"

UCLASS(BlueprintType)
class POLYPALSDEFENSE_API UEnemyDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    // 전투 능력치
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
    FEnemyStatData Stats;

    // 보상 및 패널티
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
    FEnemyRewardData Reward;

    // 시각적 연출
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
    FEnemyVisualData Visual;
};
