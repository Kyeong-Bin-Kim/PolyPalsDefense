#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
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

    virtual void BeginPlay() override;

    void InitializeWithData(UEnemyDataAsset* InDataAsset, USplineComponent* InSpline);

    void InitializeFromAssetId(const FPrimaryAssetId& AssetId, USplineComponent* InSpline);

    void ApplySlow(float Ratio, float Duration);

    void ApplyStun(float Duration);

protected:
    UFUNCTION()
    void HandleEnemyDeath();

    UPROPERTY(VisibleAnywhere, Category = "Components")
    USceneComponent* Root;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    USkeletalMeshComponent* Mesh;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UArrowComponent* DirectionIndicator;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UEnemySplineMovementComponent* SplineMovement;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UEnemyStatusComponent* Status;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
    UEnemyDataAsset* EnemyData;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
    FRotator MeshRotation = FRotator(0.f, -90.f, 0.f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
    FVector MeshScale = FVector(1.f);

    float BaseMoveSpeed;
};
