#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimInstance.generated.h"

UCLASS()
class POLYPALSDEFENSE_API UEnemyAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    // 외부에서 이동속도 값을 받아와 애니메이션에 반영
    UFUNCTION(BlueprintCallable, Category = "Animation")
    void SetSpeed(float InSpeed);

    // 애니메이션 블루프린트에서 참조할 속도
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
    float Speed;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
    bool bIsMoving = false;

protected:
    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};
