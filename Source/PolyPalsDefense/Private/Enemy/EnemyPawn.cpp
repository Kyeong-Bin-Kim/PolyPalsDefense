#include "EnemyPawn.h"
#include "Enemy/EnemyDataAsset.h"
#include "EnemySplineMovementComponent.h"
#include "EnemyStatusComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Animation/AnimInstance.h"
#include "AssetManagement/PolyPalsDefenseAssetManager.h"
#include "Kismet/GameplayStatics.h"
#include "WaveManager.h"

AEnemyPawn::AEnemyPawn()
{
    PrimaryActorTick.bCanEverTick = false;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);

    DirectionIndicator = CreateDefaultSubobject<UArrowComponent>(TEXT("Direction"));
    DirectionIndicator->SetupAttachment(RootComponent);

    SplineMovement = CreateDefaultSubobject<UEnemySplineMovementComponent>(TEXT("SplineMovement"));
    Status = CreateDefaultSubobject<UEnemyStatusComponent>(TEXT("Status"));

    Mesh->SetRelativeRotation(MeshRotation);
}

void AEnemyPawn::BeginPlay()
{
    Super::BeginPlay();
}

void AEnemyPawn::InitializeWithData(UEnemyDataAsset* InDataAsset, USplineComponent* InSpline, float HealthMultiplier, float SpeedMultiplier, FVector Scale)
{
    if (!InDataAsset) return;

    EnemyData = InDataAsset;
    RuntimeStats = FEnemyRuntimeStats(EnemyData->Stats);

    if (bIsBoss)
    {
        RuntimeStats.ApplyMultiplier(HealthMultiplier, SpeedMultiplier);
    }

    if (EnemyData->Visual.Mesh)
    {
        Mesh->SetSkeletalMesh(EnemyData->Visual.Mesh);
    }

    if (EnemyData->Visual.AnimBPClass)
    {
        Mesh->SetAnimInstanceClass(EnemyData->Visual.AnimBPClass);
    }

    Mesh->SetRelativeScale3D(Scale);

    BaseMoveSpeed = RuntimeStats.MoveSpeed;
    SplineMovement->Initialize(InSpline, BaseMoveSpeed);

    if (Status)
    {
        Status->Initialize(RuntimeStats.MaxHealth);
        Status->OnEnemyDied.AddDynamic(this, &AEnemyPawn::HandleEnemyDeath);
    }
}

void AEnemyPawn::InitializeFromAssetId(const FPrimaryAssetId& AssetId, USplineComponent* InSpline, float HealthMultiplier, float SpeedMultiplier, FVector Scale)
{
    UPrimaryDataAsset* Loaded = UPolyPalsDefenseAssetManager::Get().LoadPrimaryDataAsset(AssetId);

    if (!Loaded)
    {
        UE_LOG(LogTemp, Error, TEXT("EnemyPawn: Failed to load asset: %s"), *AssetId.ToString());
        return;
    }

    if (UEnemyDataAsset* Casted = Cast<UEnemyDataAsset>(Loaded))
    {
        InitializeWithData(Casted, InSpline, HealthMultiplier, SpeedMultiplier, Scale);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("EnemyPawn: Asset loaded but cast failed: %s"), *AssetId.ToString());
    }
}

void AEnemyPawn::ApplySlow(float Ratio, float Duration)
{
    if (Status)
    {
        Status->ApplySlow(Ratio, Duration);
    }
}

void AEnemyPawn::ApplyStun(float Duration)
{
    if (Status)
    {
        Status->ApplyStun(Duration);
    }
}

void AEnemyPawn::HandleEnemyDeath()
{
    Destroy(); // 향후 풀링 구조 도입 시 Release로 교체 가능
}

bool AEnemyPawn::IsBoss() const
{
    return bIsBoss;
}

void AEnemyPawn::ReachGoal()
{
    AWaveManager* Manager = Cast<AWaveManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AWaveManager::StaticClass()));
    if (Manager)
    {
        Manager->HandleEnemyReachedGoal(this);
    }
}