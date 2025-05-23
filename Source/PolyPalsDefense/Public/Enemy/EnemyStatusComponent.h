#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyStatusComponent.generated.h"

// 사망 이벤트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyDied);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class POLYPALSDEFENSE_API UEnemyStatusComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UEnemyStatusComponent();

    // 초기화
    void Initialize(float InMaxHealth, float InBaseMoveSpeed);

    // 데미지 및 상태 이상
    void TakeDamage(float DamageAmount);
    void ApplyStun(float Duration);
    void ApplySlow(float Ratio, float Duration);

    // === Getter (UI 연동용) ===
    float GetCurrentHealth() const { return CurrentHealth; }
    float GetMaxHealth() const { return MaxHealth; }
    float GetHealthRatio() const { return MaxHealth > 0.f ? CurrentHealth / MaxHealth : 0.f; }

    bool IsDead() const { return CurrentHealth <= 0.f; }
    bool IsStunned() const { return bIsStunned; }
    bool IsSlowed() const { return bIsSlowed; }

    float GetEffectiveMoveSpeed() const;

    // 사망 이벤트 바인딩용
    UPROPERTY(BlueprintAssignable)
    FOnEnemyDied OnEnemyDied;

protected:
    virtual void BeginPlay() override;

    // === Replication ===
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION()
    void OnRep_CurrentHealth();

private:
    void Die();
    void ClearStun();
    void ClearSlow();

    // === 상태 데이터 ===
    UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth, VisibleAnywhere)
    float CurrentHealth;

    UPROPERTY(EditAnywhere)
    float MaxHealth;

    UPROPERTY()
    float BaseMoveSpeed;

    UPROPERTY()
    float SlowRatio;

    UPROPERTY()
    bool bIsStunned;

    UPROPERTY()
    bool bIsSlowed;

    FTimerHandle StunTimerHandle;
    FTimerHandle SlowTimerHandle;
};