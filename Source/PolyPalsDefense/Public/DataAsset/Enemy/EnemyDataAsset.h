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

    // ?袁る떮 ?貫?곁㎉?
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
    FEnemyStatData Stats;

    // 癰귣똻湲?獄???ㅺ섯??
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
    FEnemyRewardData Reward;

    // ??볦퍟???怨쀭뀱
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
    FEnemyVisualData Visual;
};
