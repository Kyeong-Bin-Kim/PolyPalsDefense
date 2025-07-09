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

    // ?붾뱶???ㅽ룿????珥덇린???묒뾽
    virtual void BeginPlay() override;

    // ?곗씠???먯뀑?쇰줈 珥덇린??
    void InitializeWithData(UEnemyDataAsset* InDataAsset, USplineComponent* InSpline, float HealthMultiplier, float SpeedMultiplier, FVector Scale);

    void RewardGoldToPlayer();

    // AssetManager濡쒕????먯뀑 ID濡?珥덇린??
    void InitializeFromAssetId(const FPrimaryAssetId& AssetId, USplineComponent* InSpline, float HealthMultiplier, float SpeedMultiplier, FVector Scale);

    // ?몃??먯꽌 ?곕?吏 ?곸슜 ?붿껌???ъ슜??以묎컻 ?⑥닔
    UFUNCTION(BlueprintCallable, Category = "Enemy")
    void ReceiveDamage(float DamageAmount);

    // 蹂댁뒪 ?щ?
    bool IsBoss() const;

    // ?щ줈???곹깭 ?곸슜
    void ApplySlow(float Ratio, float Duration);

    // ?ㅽ꽩 ?곹깭 ?곸슜
    void ApplyStun(float Duration);

    // 紐⑺몴 吏???꾨떖 ???몄텧
    void ReachGoal();

    // ?앸챸?μ씠 0???섏뿀????泥섎━
    UFUNCTION()
    void HandleEnemyDeath();

    // ?쒖꽦???곹깭 ?ㅼ젙 諛?蹂듭젣 ?몃━嫄?
    void SetIsActive(bool bNewActive);

    // ?꾩옱 ?쒖꽦???곹깭 議고쉶
    bool GetIsActive() const;

    // ?먮꼫誘??곗씠???ㅼ젙
    void SetEnemyData(UEnemyDataAsset* InData);

    // ?꾩옱 ?먮꼫誘??곗씠??議고쉶
    UEnemyDataAsset* GetEnemyData() const;

public:
    // === UI??Getter ===
    // 理쒕? 泥대젰
    float GetMaxHealth() const;

    // ?꾩옱 泥대젰
    float GetHealth() const;

    // ?щ줈???곹깭 ?щ?
    bool IsStunned() const;

	// ?ㅽ꽩 ?곹깭 ?щ?
    bool IsSlowed() const;

protected:
    // 蹂듭젣???쒖꽦???곹깭 諛섏쓳
    UFUNCTION()
    void OnRep_IsActive();

    // 蹂듭젣???곗씠???먯궛 諛섏쓳
    UFUNCTION()
    void OnRep_EnemyData();

	// 蹂듭젣??援ъ껜 肄쒕━??諛섏쓳
    UFUNCTION()
    void OnRep_Scale();

	// 蹂듭젣???대룞 ?띾룄 諛섏쓳
    UFUNCTION()
    void OnRep_MoveSpeed();

    // 蹂듭젣 ?깅줉
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // === 而댄룷?뚰듃 ===

    // 猷⑦듃 而댄룷?뚰듃
    UPROPERTY(VisibleAnywhere, Category = "Components")
    USceneComponent* Root;

    // ?ㅼ펷?덊깉 硫붿떆 (?쒓컖, ?좊땲硫붿씠??
    UPROPERTY(VisibleAnywhere, Category = "Components")
    USkeletalMeshComponent* Mesh;

    // 援ъ껜 肄쒕━??
    UPROPERTY(VisibleAnywhere, Category = "Components")
    USphereComponent* CollisionSphere;

    // 諛⑺뼢 ?쒖떆???붿궡??
    UPROPERTY(VisibleAnywhere, Category = "Components")
    UArrowComponent* DirectionIndicator;

    // ?ㅽ뵆?쇱씤 ?대룞 泥섎━
    UPROPERTY(VisibleAnywhere, Category = "Components")
    UEnemySplineMovementComponent* SplineMovement;

    // 泥대젰, ?щ줈?? ?ㅽ꽩 ?곹깭 愿由?
    UPROPERTY(VisibleAnywhere, Category = "Components")
    UEnemyStatusComponent* Status;

    // 泥대젰諛?
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UEnemyHealthBarWidget> EnemyHealthBarWidgetClass;

    // 硫붿떆 ?뚯쟾 湲곕낯媛?
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
    FRotator MeshRotation = FRotator(0.f, -90.f, 0.f);

    // 援ъ껜 肄쒕━??諛섏?由?
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
	float SphereRadius = 120.f;

    // === 蹂듭젣 ?곹깭 ===

    // ?留??쒖꽦???щ?
    UPROPERTY(ReplicatedUsing = OnRep_IsActive)
    bool bIsActive;

    // ?먯뀑 ?곗씠??(硫붿떆, ?좊땲硫붿씠???ы븿)
    UPROPERTY(ReplicatedUsing = OnRep_EnemyData)
    UEnemyDataAsset* EnemyData;

    // 硫붿떆 ?ш린
    UPROPERTY(ReplicatedUsing = OnRep_Scale)
    FVector ReplicatedScale;

    // ?대룞?띾룄
    UPROPERTY(ReplicatedUsing = OnRep_MoveSpeed)
    float ReplicatedMoveSpeed;

    // 蹂댁뒪 ?щ?
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    bool bIsBoss = false;
public:

    UPROPERTY(VisibleAnywhere, Category = "UI")
    UWidgetComponent* HealthBarWidget;

private:
    // ?ㅼ떆媛??ㅽ꺈 (泥대젰, ?띾룄 ??
    FEnemyRuntimeStats RuntimeStats;
};