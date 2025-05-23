#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyStatData.h"
#include "Enemy/EnemyRewardData.h"
#include "Enemy/EnemyVisualData.h"
#include "EnemyDataAsset.generated.h"

UCLASS()
class POLYPALSDEFENSE_API UEnemyDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    FPrimaryAssetId GetPrimaryAssetId() const override
    {
        return FPrimaryAssetId("EnemyDataAsset", GetFName());
    }

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
