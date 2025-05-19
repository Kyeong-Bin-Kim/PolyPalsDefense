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

    // 이동 초기화 (스플라인, 속도)
    void Initialize(USplineComponent* InSpline, float InMoveSpeed);

    /** 이동속도 변경 (슬로우 등) */
    void SetMoveSpeed(float NewSpeed);

    /** 이동 정지 여부 설정 (스턴 등) */
    void SetPaused(bool bPause);

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