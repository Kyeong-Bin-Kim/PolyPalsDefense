#include "Enemy/EnemyPoolComponent.h"
#include "Enemy/EnemyDataAsset.h"
#include "Enemy/EnemyPawn.h"
#include "Components/SplineComponent.h"
#include "Map/StageActor.h"
#include "Map/WavePlanRow.h"
#include "AssetManagement/PolyPalsDefenseAssetManager.h"
#include "Engine/World.h"
#include "EngineUtils.h"

UEnemyPoolComponent::UEnemyPoolComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UEnemyPoolComponent::BeginPlay()
{
    Super::BeginPlay();

    // ??뺤쒔 亦낅슦釉???袁⑤빍筌???筌?嚥≪뮇彛???쎄땁
    if (!GetOwner() || !GetOwner()->HasAuthority())
        return;
}

AEnemyPawn* UEnemyPoolComponent::AcquireEnemy(const FPrimaryAssetId& AssetId, USplineComponent* InSpline, float HealthMultiplier, float SpeedMultiplier, bool bIsBoss, FVector Scale)
{
    AEnemyPawn* EnemyPawn = nullptr;

    // ??????? ?紐꾨뮞??곷뮞揶쎛 ??됱몵筌??????
    if (EnemyPool.Contains(AssetId) && EnemyPool[AssetId].Num() > 0)
    {
        EnemyPawn = EnemyPool[AssetId].Pop();

        // ???????뽯퓠???袁⑹읈 ?λ뜃由??
        EnemyPawn->InitializeFromAssetId(AssetId, InSpline, HealthMultiplier, SpeedMultiplier, Scale);
    }
    else
    {
        // ??????곸몵筌???덉쨮 ??밴쉐
        EnemyPawn = CreateNewEnemy(AssetId, InSpline, HealthMultiplier, SpeedMultiplier, bIsBoss, Scale);
    }

    if (EnemyPawn)
    {
        // ?怨밴묶 ??뽮쉐??筌ｌ꼶??(揶쎛??뽮쉐/?겸뫖猷?????釉?
        EnemyPawn->SetIsActive(true);
    }

    return EnemyPawn;
}

void UEnemyPoolComponent::ReleaseEnemy(AEnemyPawn* Enemy)
{
    if (!Enemy)
        return;

    // ?怨밴묶 ??쑵??源딆넅 筌ｌ꼶??(揶쎛??뽮쉐/?겸뫖猷?????釉?
    Enemy->SetIsActive(false);

    // ????獄쏆꼹??
    if (UEnemyDataAsset* Data = Enemy->GetEnemyData())
    {
        EnemyPool.FindOrAdd(Data->GetPrimaryAssetId()).Add(Enemy);
    }
}

AEnemyPawn* UEnemyPoolComponent::CreateNewEnemy(const FPrimaryAssetId& AssetId, USplineComponent* InSpline, float HealthMultiplier, float SpeedMultiplier, bool bIsBoss, FVector Scale)
{
    if (!EnemyClass || !GetWorld())
        return nullptr;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AEnemyPawn* NewEnemyPawn = GetWorld()->SpawnActor<AEnemyPawn>(
        EnemyClass,
        FVector(5000.f, 0.f, 5000.f),
        FRotator::ZeroRotator,
        SpawnParams
    );

    if (NewEnemyPawn)
    {
        NewEnemyPawn->InitializeFromAssetId(AssetId, InSpline, HealthMultiplier, SpeedMultiplier, Scale);
        NewEnemyPawn->SetIsActive(true);
    }

    return NewEnemyPawn;
}
