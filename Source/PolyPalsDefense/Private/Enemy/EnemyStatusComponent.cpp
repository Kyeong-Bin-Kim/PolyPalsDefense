#include "EnemyStatusComponent.h"
#include "TimerManager.h"
#include "GameFramework/Actor.h"

UEnemyStatusComponent::UEnemyStatusComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    MaxHealth = 100.f;
    CurrentHealth = MaxHealth;
    SlowRatio = 0.5f;
    bIsStunned = false;
}

void UEnemyStatusComponent::BeginPlay()
{
    Super::BeginPlay();
    CurrentHealth = MaxHealth;
}

void UEnemyStatusComponent::Initialize(float InMaxHealth, float InBaseMoveSpeed)
{
    MaxHealth = InMaxHealth;
    CurrentHealth = MaxHealth;
    BaseMoveSpeed = InBaseMoveSpeed;
}

float UEnemyStatusComponent::GetEffectiveMoveSpeed() const
{
    return bIsStunned ? 0.f : BaseMoveSpeed * SlowRatio;
}

void UEnemyStatusComponent::TakeDamage(float DamageAmount)
{
    if (DamageAmount <= 0.f || CurrentHealth <= 0.f) return;

    CurrentHealth -= DamageAmount;
    if (CurrentHealth <= 0.f)
    {
        Die();
    }
}

void UEnemyStatusComponent::ApplyStun(float Duration)
{
    if (Duration <= 0.f) return;

    bIsStunned = true;
    GetWorld()->GetTimerManager().ClearTimer(StunTimerHandle);
    GetWorld()->GetTimerManager().SetTimer(StunTimerHandle, this, &UEnemyStatusComponent::ClearStun, Duration, false);
}

void UEnemyStatusComponent::ApplySlow(float Ratio, float Duration)
{
    SlowRatio = FMath::Clamp(Ratio, 0.f, 1.f);
    GetWorld()->GetTimerManager().ClearTimer(SlowTimerHandle);
    GetWorld()->GetTimerManager().SetTimer(SlowTimerHandle, this, &UEnemyStatusComponent::ClearSlow, Duration, false);
}

void UEnemyStatusComponent::ClearStun()
{
    bIsStunned = false;
}

void UEnemyStatusComponent::ClearSlow()
{
    SlowRatio = 1.f;
}

void UEnemyStatusComponent::Die()
{
    OnEnemyDied.Broadcast();
}
