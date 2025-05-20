#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StageActor.generated.h"

class USplineComponent;

UCLASS()
class POLYPALSDEFENSE_API AStageActor : public AActor
{
    GENERATED_BODY()

public:
    AStageActor();

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USplineComponent* PathSpline;

    USplineComponent* GetSpline() const { return PathSpline; }
};
