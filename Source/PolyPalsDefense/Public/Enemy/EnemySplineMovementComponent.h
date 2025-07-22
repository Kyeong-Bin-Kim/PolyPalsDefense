#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemySplineMovementComponent.generated.h"

class USplineComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class POLYPALSDEFENSE_API UEnemySplineMovementComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UEnemySplineMovementComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // ??猷??λ뜃由??(??쎈탣??깆뵥, ??얜즲)
    void Initialize(USplineComponent* InSpline, float InMoveSpeed);

    // ??猷??얜즲 癰궰野?(??以??
    void SetMoveSpeed(float NewSpeed);

    // ??猷??類? ??? ??쇱젟 (??쎄쉘)
    void SetPaused(bool bPause);

    USplineComponent* GetSpline() const { return Spline; }

    float GetCurrentDistance() const { return CurrentDistance; }

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY()
    USplineComponent* Spline;

    float CurrentDistance;
    float MoveSpeed;
    bool bIsPaused;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float EndTolerance = 10.f;
};