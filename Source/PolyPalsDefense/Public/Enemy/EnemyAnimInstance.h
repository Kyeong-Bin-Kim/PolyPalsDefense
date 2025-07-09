#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimInstance.generated.h"

UCLASS()
class POLYPALSDEFENSE_API UEnemyAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    // ?몃??먯꽌 ?대룞?띾룄 媛믪쓣 諛쏆븘? ?좊땲硫붿씠?섏뿉 諛섏쁺
    UFUNCTION(BlueprintCallable, Category = "Animation")
    void SetSpeed(float InSpeed);

    // ?좊땲硫붿씠??釉붾（?꾨┛?몄뿉??李몄“???띾룄
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
    float Speed;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
    bool bIsMoving = false;

protected:
    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};
