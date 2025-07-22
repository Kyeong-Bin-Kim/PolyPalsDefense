#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimInstance.generated.h"

UCLASS()
class POLYPALSDEFENSE_API UEnemyAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    // ?紐??癒?퐣 ??猷??얜즲 揶쏅???獄쏆룇釉?? ?醫딅빍筌롫뗄???뤿퓠 獄쏆꼷??
    UFUNCTION(BlueprintCallable, Category = "Animation")
    void SetSpeed(float InSpeed);

    // ?醫딅빍筌롫뗄????됰뗀竊?袁ⓥ뵛?紐꾨퓠??筌〓챷?????얜즲
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
    float Speed;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
    bool bIsMoving = false;

protected:
    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};
