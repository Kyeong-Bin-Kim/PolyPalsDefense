#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyStatusComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyDied);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class POLYPALSDEFENSE_API UEnemyStatusComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UEnemyStatusComponent();

    /** 체력 초기화 */
    void Initialize(float InMaxHealth);

    /** 데미지 적용 */
    void TakeDamage(float DamageAmount);

    /** 스턴 적용 (지속 시간 포함) */
    void ApplyStun(float Duration);

    /** 슬로우 적용 (비율과 지속시간) */
    void ApplySlow(float Ratio, float Duration);

    /** 사망 델리게이트 */
    UPROPERTY(BlueprintAssignable)
    FOnEnemyDied OnEnemyDied;

    /** 현재 체력 */
    UPROPERTY(BlueprintReadOnly)
    float CurrentHealth;

    /** 최대 체력 */
    UPROPERTY(BlueprintReadOnly, EditAnywhere)
    float MaxHealth;

    /** 현재 슬로우 비율 (0 ~ 1) */
    UPROPERTY(BlueprintReadOnly)
    float SlowRatio;

    /** 현재 스턴 여부 */
    UPROPERTY(BlueprintReadOnly)
    bool bIsStunned;

protected:
    virtual void BeginPlay() override;

private:
    FTimerHandle StunTimerHandle;
    FTimerHandle SlowTimerHandle;

    void ClearStun();
    void ClearSlow();
    void Die();
};
