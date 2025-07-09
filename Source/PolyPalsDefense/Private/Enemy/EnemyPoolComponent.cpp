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

    // ?쒕쾭 沅뚰븳???꾨땲硫??留?濡쒖쭅 ?ㅽ궢
    if (!GetOwner() || !GetOwner()->HasAuthority())
        return;
}

AEnemyPawn* UEnemyPoolComponent::AcquireEnemy(const FPrimaryAssetId& AssetId, USplineComponent* InSpline, float HealthMultiplier, float SpeedMultiplier, bool bIsBoss, FVector Scale)
{
    AEnemyPawn* EnemyPawn = nullptr;

    // ????⑥? ?몄뒪?댁뒪媛 ?덉쑝硫??ы솢??
    if (EnemyPool.Contains(AssetId) && EnemyPool[AssetId].Num() > 0)
    {
        EnemyPawn = EnemyPool[AssetId].Pop();

        // ?ы솢???쒖뿉???꾩쟾 珥덇린??
        EnemyPawn->InitializeFromAssetId(AssetId, InSpline, HealthMultiplier, SpeedMultiplier, Scale);
    }
    else
    {
        // ????놁쑝硫??덈줈 ?앹꽦
        EnemyPawn = CreateNewEnemy(AssetId, InSpline, HealthMultiplier, SpeedMultiplier, bIsBoss, Scale);
    }

    if (EnemyPawn)
    {
        // ?곹깭 ?쒖꽦??泥섎━ (媛?쒖꽦/異⑸룎/???ы븿)
        EnemyPawn->SetIsActive(true);
    }

    return EnemyPawn;
}

void UEnemyPoolComponent::ReleaseEnemy(AEnemyPawn* Enemy)
{
    if (!Enemy)
        return;

    // ?곹깭 鍮꾪솢?깊솕 泥섎━ (媛?쒖꽦/異⑸룎/???ы븿)
    Enemy->SetIsActive(false);

    // ???諛섑솚
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
