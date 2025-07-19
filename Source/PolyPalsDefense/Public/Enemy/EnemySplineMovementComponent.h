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

    // ?대룞 珥덇린??(?ㅽ뵆?쇱씤, ?띾룄)
    void Initialize(USplineComponent* InSpline, float InMoveSpeed);

    // ?대룞?띾룄 蹂寃?(?щ줈??
    void SetMoveSpeed(float NewSpeed);

    // ?대룞 ?뺤? ?щ? ?ㅼ젙 (?ㅽ꽩)
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