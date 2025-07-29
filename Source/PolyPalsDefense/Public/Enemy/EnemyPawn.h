#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Enemy/EnemyRuntimeStats.h"
#include "Components/WidgetComponent.h"
#include "EnemyPawn.generated.h"

class USplineComponent;
class UEnemyDataAsset;
class USkeletalMeshComponent;
class USphereComponent;
class UArrowComponent;
class UEnemySplineMovementComponent;
class UEnemyStatusComponent;
class UAnimInstance;

UCLASS()
class POLYPALSDEFENSE_API AEnemyPawn : public APawn
{
    GENERATED_BODY()

public:
    AEnemyPawn();

    virtual void BeginPlay() override;

    virtual void Tick(float DeltaTime) override;

    void InitializeWithData(UEnemyDataAsset* InDataAsset, USplineComponent* InSpline, float HealthMultiplier, float SpeedMultiplier, FVector Scale);

    void RewardGoldToPlayer();

    void InitializeFromAssetId(const FPrimaryAssetId& AssetId, USplineComponent* InSpline, float HealthMultiplier, float SpeedMultiplier, FVector Scale);

    UFUNCTION(BlueprintCallable, Category = "Enemy")
    void ReceiveDamage(float DamageAmount);

    bool IsBoss() const;

    void ApplySlow(float Ratio, float Duration);

    void ApplyStun(float Duration);

    void ReachGoal();

    UFUNCTION()
    void HandleEnemyDeath();

    void SetIsActive(bool bNewActive);

    bool GetIsActive() const;

    void SetEnemyData(UEnemyDataAsset* InData);

    UEnemyDataAsset* GetEnemyData() const;

public:
    float GetMaxHealth() const;

    float GetHealth() const;

    bool IsStunned() const;

    bool IsSlowed() const;

    UEnemyStatusComponent* GetStatus() const { return Status; }

    float GetDistanceAlongPath() const { return DistanceAlongPath; }

protected:
    UFUNCTION()
    void OnRep_IsActive();

    UFUNCTION()
    void OnRep_EnemyData();

    UFUNCTION()
    void OnRep_Scale();

    UFUNCTION()
    void OnRep_MoveSpeed();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    USceneComponent* Root;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    USkeletalMeshComponent* Mesh;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    USphereComponent* CollisionSphere;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UArrowComponent* DirectionIndicator;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UEnemySplineMovementComponent* SplineMovement;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UEnemyStatusComponent* Status;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UEnemyHealthBarWidget> EnemyHealthBarWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
    FRotator MeshRotation = FRotator(0.f, -90.f, 0.f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
	float SphereRadius = 120.f;

    UPROPERTY(VisibleAnywhere, Category = "Movement")
    float DistanceAlongPath = 0.f;

    UPROPERTY(ReplicatedUsing = OnRep_IsActive)
    bool bIsActive;

    UPROPERTY(ReplicatedUsing = OnRep_EnemyData)
    UEnemyDataAsset* EnemyData;

    UPROPERTY(ReplicatedUsing = OnRep_Scale)
    FVector ReplicatedScale;

    UPROPERTY(ReplicatedUsing = OnRep_MoveSpeed)
    float ReplicatedMoveSpeed;

public:
    UPROPERTY(VisibleAnywhere, Category = "UI")
    UWidgetComponent* HealthBarWidget;

    UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    bool bIsBoss = false;

private:
    FEnemyRuntimeStats RuntimeStats;
};