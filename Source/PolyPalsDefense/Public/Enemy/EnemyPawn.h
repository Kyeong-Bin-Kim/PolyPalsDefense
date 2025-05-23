#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Enemy/EnemyRuntimeStats.h"
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

    // 월드에 스폰될 때 초기화 작업
    virtual void BeginPlay() override;

    // 데이터 에셋으로 초기화
    void InitializeWithData(UEnemyDataAsset* InDataAsset, USplineComponent* InSpline, float HealthMultiplier, float SpeedMultiplier, FVector Scale);

    // AssetManager로부터 에셋 ID로 초기화
    void InitializeFromAssetId(const FPrimaryAssetId& AssetId, USplineComponent* InSpline, float HealthMultiplier, float SpeedMultiplier, FVector Scale);

    // 보스 여부
    bool IsBoss() const;

    // 슬로우 상태 적용
    void ApplySlow(float Ratio, float Duration);

    // 스턴 상태 적용
    void ApplyStun(float Duration);

    // 목표 지점 도달 시 호출
    void ReachGoal();

    // 생명력이 0이 되었을 때 처리
    UFUNCTION()
    void HandleEnemyDeath();

    // 활성화 상태 설정 및 복제 트리거
    void SetIsActive(bool bNewActive);

    // 현재 활성화 상태 조회
    bool GetIsActive() const;

    // 에너미 데이터 설정
    void SetEnemyData(UEnemyDataAsset* InData);

    // 현재 에너미 데이터 조회
    UEnemyDataAsset* GetEnemyData() const;

public:
    // === UI용 Getter ===
    // 최대 체력
    float GetMaxHealth() const;

    // 현재 체력
    float GetHealth() const;

    // 슬로우 상태 여부
    bool IsStunned() const;

	// 스턴 상태 여부
    bool IsSlowed() const;

protected:
    // 복제된 활성화 상태 반응
    UFUNCTION()
    void OnRep_IsActive();

    // 복제된 데이터 자산 반응
    UFUNCTION()
    void OnRep_EnemyData();

	// 복제된 구체 콜리전 반응
    UFUNCTION()
    void OnRep_Scale();

	// 복제된 이동 속도 반응
    UFUNCTION()
    void OnRep_MoveSpeed();

    // 복제 등록
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // === 컴포넌트 ===

    // 루트 컴포넌트
    UPROPERTY(VisibleAnywhere, Category = "Components")
    USceneComponent* Root;

    // 스켈레탈 메시 (시각, 애니메이션)
    UPROPERTY(VisibleAnywhere, Category = "Components")
    USkeletalMeshComponent* Mesh;

    // 구체 콜리전
    UPROPERTY(VisibleAnywhere, Category = "Components")
    USphereComponent* CollisionSphere;

    // 방향 표시용 화살표
    UPROPERTY(VisibleAnywhere, Category = "Components")
    UArrowComponent* DirectionIndicator;

    // 스플라인 이동 처리
    UPROPERTY(VisibleAnywhere, Category = "Components")
    UEnemySplineMovementComponent* SplineMovement;

    // 체력, 슬로우, 스턴 상태 관리
    UPROPERTY(VisibleAnywhere, Category = "Components")
    UEnemyStatusComponent* Status;

    // 메시 회전 기본값
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
    FRotator MeshRotation = FRotator(0.f, -90.f, 0.f);

    // 구체 콜리전 반지름
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
	float SphereRadius = 120.f;

    // === 복제 상태 ===

    // 풀링 활성화 여부
    UPROPERTY(ReplicatedUsing = OnRep_IsActive)
    bool bIsActive;

    // 에셋 데이터 (메시, 애니메이션 포함)
    UPROPERTY(ReplicatedUsing = OnRep_EnemyData)
    UEnemyDataAsset* EnemyData;

    // 메시 크기
    UPROPERTY(ReplicatedUsing = OnRep_Scale)
    FVector ReplicatedScale;

    // 이동속도
    UPROPERTY(ReplicatedUsing = OnRep_MoveSpeed)
    float ReplicatedMoveSpeed;

    // 보스 여부
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    bool bIsBoss = false;

private:
    // 실시간 스탯 (체력, 속도 등)
    FEnemyRuntimeStats RuntimeStats;
};