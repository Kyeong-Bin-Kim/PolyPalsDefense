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
    Mesh->SetRelativeScale3D(MeshScale);
}

void AEnemyPawn::BeginPlay()
{
    Super::BeginPlay();

    if (EnemyData)
    {
        InitializeWithData(EnemyData, nullptr); // 데이터 에셋 테스트 용
    }
}

void AEnemyPawn::InitializeWithData(UEnemyDataAsset* InDataAsset, USplineComponent* InSpline)
{
    EnemyData = InDataAsset;

    if (EnemyData->Visual.Mesh)
    {
        Mesh->SetSkeletalMesh(EnemyData->Visual.Mesh);
    }

    if (EnemyData->Visual.AnimBPClass)
    {
        Mesh->SetAnimInstanceClass(EnemyData->Visual.AnimBPClass);
    }

    BaseMoveSpeed = EnemyData ? EnemyData->Stats.BaseMoveSpeed : 300.f;
    SplineMovement->Initialize(InSpline, BaseMoveSpeed);

    if (Status)
    {
        Status->Initialize(EnemyData->Stats.MaxHealth);
        Status->OnEnemyDied.AddDynamic(this, &AEnemyPawn::HandleEnemyDeath); // 테스트용 사망 시 삭제
    }
}

void AEnemyPawn::InitializeFromAssetId(const FPrimaryAssetId& AssetId, USplineComponent* InSpline)
{
    UPrimaryDataAsset* Loaded = UPolyPalsDefenseAssetManager::Get().LoadPrimaryDataAsset(AssetId);

    if (!Loaded)
    {
        UE_LOG(LogTemp, Error, TEXT("EnemyPawn: Failed to load asset: %s"), *AssetId.ToString());
        return;
    }

    if (UEnemyDataAsset* Casted = Cast<UEnemyDataAsset>(Loaded))
    {
        InitializeWithData(Casted, InSpline);
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
