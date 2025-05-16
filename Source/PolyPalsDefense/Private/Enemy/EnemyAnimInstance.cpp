#include "EnemyAnimInstance.h"

void UEnemyAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();
    Speed = 0.f;
}

void UEnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);
    // Tick마다 애니메이션 파라미터 갱신 필요 시 추가 가능
}

void UEnemyAnimInstance::SetSpeed(float InSpeed)
{
    Speed = InSpeed;
    bIsMoving = (Speed > 1.f);
}
