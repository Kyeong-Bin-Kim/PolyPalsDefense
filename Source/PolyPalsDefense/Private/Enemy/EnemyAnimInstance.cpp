#include "EnemyAnimInstance.h"

void UEnemyAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();
    Speed = 0.f;
}

void UEnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);
}

void UEnemyAnimInstance::SetSpeed(float InSpeed)
{
    Speed = InSpeed;
    bIsMoving = (Speed > 1.f);
}
