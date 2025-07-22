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

    // ?遺얜굡????쎈？?????λ뜃由???臾믩씜
    virtual void BeginPlay() override;

    virtual void Tick(float DeltaTime) override;

    // ?怨쀬뵠???癒???곗쨮 ?λ뜃由??
    void InitializeWithData(UEnemyDataAsset* InDataAsset, USplineComponent* InSpline, float HealthMultiplier, float SpeedMultiplier, FVector Scale);

    void RewardGoldToPlayer();

    // AssetManager嚥≪뮆????癒??ID嚥??λ뜃由??
    void InitializeFromAssetId(const FPrimaryAssetId& AssetId, USplineComponent* InSpline, float HealthMultiplier, float SpeedMultiplier, FVector Scale);

    // ?紐??癒?퐣 ?怨?筌왖 ?怨몄뒠 ?遺욧퍕???????餓λ쵌而???λ땾
    UFUNCTION(BlueprintCallable, Category = "Enemy")
    void ReceiveDamage(float DamageAmount);

    // 癰귣똻?????
    bool IsBoss() const;

    // ??以???怨밴묶 ?怨몄뒠
    void ApplySlow(float Ratio, float Duration);

    // ??쎄쉘 ?怨밴묶 ?怨몄뒠
    void ApplyStun(float Duration);

    // 筌뤴뫚紐?筌왖???袁⑤뼎 ???紐꾪뀱
    void ReachGoal();

    // ??몄구?關??0????뤿?????筌ｌ꼶??
    UFUNCTION()
    void HandleEnemyDeath();

    // ??뽮쉐???怨밴묶 ??쇱젟 獄?癰귣벊???紐꺿봺椰?
    void SetIsActive(bool bNewActive);

    // ?袁⑹삺 ??뽮쉐???怨밴묶 鈺곌퀬??
    bool GetIsActive() const;

    // ?癒?섐沃??怨쀬뵠????쇱젟
    void SetEnemyData(UEnemyDataAsset* InData);

    // ?袁⑹삺 ?癒?섐沃??怨쀬뵠??鈺곌퀬??
    UEnemyDataAsset* GetEnemyData() const;

public:
    // === UI??Getter ===
    // 筌ㅼ뮆? 筌ｋ???
    float GetMaxHealth() const;

    // ?袁⑹삺 筌ｋ???
    float GetHealth() const;

    // ??以???怨밴묶 ???
    bool IsStunned() const;

	// ??쎄쉘 ?怨밴묶 ???
    bool IsSlowed() const;

    UEnemyStatusComponent* GetStatus() const { return Status; }

    float GetDistanceAlongPath() const { return DistanceAlongPath; }

protected:
    // 癰귣벊?????뽮쉐???怨밴묶 獄쏆꼷??
    UFUNCTION()
    void OnRep_IsActive();

    // 癰귣벊????怨쀬뵠???癒?텦 獄쏆꼷??
    UFUNCTION()
    void OnRep_EnemyData();

	// 癰귣벊????닌딄퍥 ?꾩뮆???獄쏆꼷??
    UFUNCTION()
    void OnRep_Scale();

	// 癰귣벊?????猷???얜즲 獄쏆꼷??
    UFUNCTION()
    void OnRep_MoveSpeed();

    // 癰귣벊???源낆쨯
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // === ?뚮똾猷??곕뱜 ===

    // ?룐뫂???뚮똾猷??곕뱜
    UPROPERTY(VisibleAnywhere, Category = "Components")
    USceneComponent* Root;

    // ??쇳렩??딄퉱 筌롫뗄??(??볦퍟, ?醫딅빍筌롫뗄???
    UPROPERTY(VisibleAnywhere, Category = "Components")
    USkeletalMeshComponent* Mesh;

    // ?닌딄퍥 ?꾩뮆???
    UPROPERTY(VisibleAnywhere, Category = "Components")
    USphereComponent* CollisionSphere;

    // 獄쎻뫚堉???뽯뻻???遺욧땀??
    UPROPERTY(VisibleAnywhere, Category = "Components")
    UArrowComponent* DirectionIndicator;

    // ??쎈탣??깆뵥 ??猷?筌ｌ꼶??
    UPROPERTY(VisibleAnywhere, Category = "Components")
    UEnemySplineMovementComponent* SplineMovement;

    // 筌ｋ??? ??以?? ??쎄쉘 ?怨밴묶 ?온??
    UPROPERTY(VisibleAnywhere, Category = "Components")
    UEnemyStatusComponent* Status;

    // 筌ｋ??계쳸?
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UEnemyHealthBarWidget> EnemyHealthBarWidgetClass;

    // 筌롫뗄?????읈 疫꿸퀡??첎?
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
    FRotator MeshRotation = FRotator(0.f, -90.f, 0.f);

    // ?닌딄퍥 ?꾩뮆???獄쏆꼷???
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
	float SphereRadius = 120.f;

    UPROPERTY(VisibleAnywhere, Category = "Movement")
    float DistanceAlongPath = 0.f;

    // === 癰귣벊???怨밴묶 ===

    // ??筌???뽮쉐?????
    UPROPERTY(ReplicatedUsing = OnRep_IsActive)
    bool bIsActive;

    // ?癒???怨쀬뵠??(筌롫뗄?? ?醫딅빍筌롫뗄?????釉?
    UPROPERTY(ReplicatedUsing = OnRep_EnemyData)
    UEnemyDataAsset* EnemyData;

    // 筌롫뗄????由?
    UPROPERTY(ReplicatedUsing = OnRep_Scale)
    FVector ReplicatedScale;

    // ??猷??얜즲
    UPROPERTY(ReplicatedUsing = OnRep_MoveSpeed)
    float ReplicatedMoveSpeed;

    // 癰귣똻?????
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    bool bIsBoss = false;
public:

    UPROPERTY(VisibleAnywhere, Category = "UI")
    UWidgetComponent* HealthBarWidget;

private:
    // ??쇰뻻揶???쎄틛 (筌ｋ??? ??얜즲 ??
    FEnemyRuntimeStats RuntimeStats;
};