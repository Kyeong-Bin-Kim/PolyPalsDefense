#pragma once

#include "CoreMinimal.h"
#include "EnemyVisualData.generated.h"

class USkeletalMesh;
class UMaterialInterface;

USTRUCT(BlueprintType)
struct FEnemyVisualData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Visual")
    USkeletalMesh* Mesh = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Visual")
    TSubclassOf<UAnimInstance> AnimBPClass = nullptr;
};
