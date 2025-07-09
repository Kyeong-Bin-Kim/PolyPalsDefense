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

    // ?꾪닾 ?λ젰移?
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
    FEnemyStatData Stats;

    // 蹂댁긽 諛??⑤꼸??
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
    FEnemyRewardData Reward;

    // ?쒓컖???곗텧
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
    FEnemyVisualData Visual;
};
