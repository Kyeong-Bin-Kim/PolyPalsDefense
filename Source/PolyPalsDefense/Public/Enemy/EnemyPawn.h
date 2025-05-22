#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Enemy/EnemyRuntimeStats.h"
#include "EnemyPawn.generated.h"

class USplineComponent;
class UEnemyDataAsset;
class USkeletalMeshComponent;
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

    // 슬로우 상태 적용
    void ApplySlow(float Ratio, float Duration);

    // 스턴 상태 적용
    void ApplyStun(float Duration);

    // 목표 지점 도달 시 호출
    void ReachGoal();

    // 보스 여부 반환
    bool IsBoss() const;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    bool bIsBoss = false;

    // 풀링 활성화 상태, 클라이언트에 동기화
    UPROPERTY(ReplicatedUsing = OnRep_IsActive)
    bool bIsActive;

protected:
    // 복제할 프로퍼티 등록
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    // 클라이언트에서 활성화 상태 변경 시 처리
    UFUNCTION()
    void OnRep_IsActive();

    // 생명력이 0이 되었을 때 처리
    UFUNCTION()
    void HandleEnemyDeath();

    // 루트 컴포넌트 (충돌 없음)
    UPROPERTY(VisibleAnywhere, Category = "Components")
    USceneComponent* Root;

    // 스켈레탈 메시 컴포넌트
    UPROPERTY(VisibleAnywhere, Category = "Components")
    USkeletalMeshComponent* Mesh;

    // 방향 표시용 화살표 컴포넌트
    UPROPERTY(VisibleAnywhere, Category = "Components")
    UArrowComponent* DirectionIndicator;

    // 스플라인 기반 이동 컴포넌트
    UPROPERTY(VisibleAnywhere, Category = "Components")
    UEnemySplineMovementComponent* SplineMovement;

    // 상태 관리 컴포넌트 (체력, 슬로우, 스턴)
    UPROPERTY(VisibleAnywhere, Category = "Components")
    UEnemyStatusComponent* Status;

    // 에셋 데이터
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
    UEnemyDataAsset* EnemyData;

    // 메시 회전 기본값
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
    FRotator MeshRotation = FRotator(0.f, -90.f, 0.f);

private:
    // 기본 이동 속도
    float BaseMoveSpeed;
    // 런타임용 스탯
    FEnemyRuntimeStats RuntimeStats;
};
