#include "EnemyPawn.h"
#include "Enemy/EnemyDataAsset.h"
#include "EnemySplineMovementComponent.h"
#include "EnemyStatusComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "EnemyAnimInstance.h"
#include "AssetManagement/PolyPalsDefenseAssetManager.h"
#include "Kismet/GameplayStatics.h"
#include "WaveManager.h"
#include "Net/UnrealNetwork.h"

AEnemyPawn::AEnemyPawn()
{
    // 네트워크 복제 설정
    bReplicates = true;
    SetReplicateMovement(true);

    PrimaryActorTick.bCanEverTick = false;

    // 루트 컴포넌트 생성
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    // 스켈레탈 메시 생성 및 충돌 설정 (Overlap만 허용)
    Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
    Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Mesh->SetCollisionObjectType(ECC_Pawn);
    Mesh->SetCollisionResponseToAllChannels(ECR_Overlap);

    // 방향 표시용 컴포넌트
    DirectionIndicator = CreateDefaultSubobject<UArrowComponent>(TEXT("Direction"));
    DirectionIndicator->SetupAttachment(RootComponent);

    // 이동, 상태 컴포넌트 생성
    SplineMovement = CreateDefaultSubobject<UEnemySplineMovementComponent>(TEXT("SplineMovement"));
    Status = CreateDefaultSubobject<UEnemyStatusComponent>(TEXT("Status"));

    // 메시 기본 회전 적용
    Mesh->SetRelativeRotation(MeshRotation);

    // 태그 설정 (타워 공격, 스킬 판정용)
    Tags.Add(FName(TEXT("Enemy")));

    // 초기에는 비활성화 상태로 숨김
    bIsActive = false;
    SetActorHiddenInGame(true);
}

void AEnemyPawn::BeginPlay()
{
    Super::BeginPlay();
}

void AEnemyPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AEnemyPawn, bIsActive);
    DOREPLIFETIME(AEnemyPawn, EnemyData);
    DOREPLIFETIME(AEnemyPawn, ReplicatedScale);
    DOREPLIFETIME(AEnemyPawn, ReplicatedMoveSpeed);
}

void AEnemyPawn::OnRep_IsActive()
{
    SetActorHiddenInGame(!bIsActive);
    SetActorEnableCollision(bIsActive);
    SetActorTickEnabled(bIsActive);
}

void AEnemyPawn::OnRep_EnemyData()
{
    if (!EnemyData) return;

    // 메시 먼저 설정
    if (EnemyData->Visual.Mesh)
    {
        Mesh->SetSkeletalMesh(EnemyData->Visual.Mesh);
        UE_LOG(LogTemp, Warning, TEXT("[EnemyPawn] 메시 설정됨: %s"), *EnemyData->Visual.Mesh->GetName());
    }

    // 애니메이션 블루프린트 설정
    if (EnemyData->Visual.AnimBPClass)
    {
        Mesh->SetAnimInstanceClass(EnemyData->Visual.AnimBPClass);
        UE_LOG(LogTemp, Warning, TEXT("[EnemyPawn] 메시 설정됨: %s"), *EnemyData->Visual.Mesh->GetName());
        Mesh->InitAnim(true);

        // 애님 인스턴스에 속도 전달
        if (UEnemyAnimInstance* EnemyAnim = Cast<UEnemyAnimInstance>(Mesh->GetAnimInstance()))
        {
            EnemyAnim->SetSpeed(ReplicatedMoveSpeed); // 복제된 속도 전달
        }
    }

    // 복제된 스케일도 적용
    Mesh->SetRelativeScale3D(ReplicatedScale);

    UE_LOG(LogTemp, Warning, TEXT("[EnemyPawn] OnRep_EnemyData 실행됨 - 애니메이션: %s, 스케일: %s"),
        EnemyData->Visual.AnimBPClass ? *EnemyData->Visual.AnimBPClass->GetName() : TEXT("None"),
        *ReplicatedScale.ToString());
}

void AEnemyPawn::OnRep_Scale()
{
    Mesh->SetRelativeScale3D(ReplicatedScale);
}

void AEnemyPawn::OnRep_MoveSpeed()
{
    if (UEnemyAnimInstance* Anim = Cast<UEnemyAnimInstance>(Mesh->GetAnimInstance()))
    {
        Anim->SetSpeed(ReplicatedMoveSpeed);
    }
}

void AEnemyPawn::InitializeWithData(UEnemyDataAsset* InDataAsset, USplineComponent* InSpline, float HealthMultiplier, float SpeedMultiplier, FVector Scale)
{
    if (!InDataAsset) return;

    EnemyData = InDataAsset;
    ReplicatedScale = Scale;
    ReplicatedMoveSpeed = RuntimeStats.MoveSpeed;

    // 런타임 스탯 생성 및 배수 적용
    RuntimeStats = FEnemyRuntimeStats(EnemyData->Stats);
    if (bIsBoss)
    {
        RuntimeStats.ApplyMultiplier(HealthMultiplier, SpeedMultiplier);
    }

    // 메시 및 애니메이션 설정
    if (EnemyData->Visual.Mesh)
    {
        Mesh->SetSkeletalMesh(EnemyData->Visual.Mesh);
    }
    if (EnemyData->Visual.AnimBPClass)
    {
        Mesh->SetAnimInstanceClass(EnemyData->Visual.AnimBPClass);
    }

    // 스케일 적용
    Mesh->SetRelativeScale3D(ReplicatedScale);

    // 상태 컴포넌트 초기화 및 사망 이벤트 바인딩
    if (Status)
    {
        Status->Initialize(RuntimeStats.MaxHealth, RuntimeStats.MoveSpeed);
        Status->OnEnemyDied.RemoveDynamic(this, &AEnemyPawn::HandleEnemyDeath);
        Status->OnEnemyDied.AddDynamic(this, &AEnemyPawn::HandleEnemyDeath);
    }

    // 스플라인 이동 초기화 (Status에서 유효 속도 받아서)
    const float EffectiveSpeed = Status ? Status->GetEffectiveMoveSpeed() : RuntimeStats.MoveSpeed;
    SplineMovement->Initialize(InSpline, EffectiveSpeed);
}

void AEnemyPawn::InitializeFromAssetId(const FPrimaryAssetId& InAssetId, USplineComponent* InSpline, float HealthMultiplier, float SpeedMultiplier, FVector Scale)
{
    UPrimaryDataAsset* Loaded = UPolyPalsDefenseAssetManager::Get().LoadPrimaryDataAsset(InAssetId);
    if (UEnemyDataAsset* Casted = Cast<UEnemyDataAsset>(Loaded))
    {
        InitializeWithData(Casted, InSpline, HealthMultiplier, SpeedMultiplier, Scale);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("EnemyPawn: Asset loaded but cast failed: %s"), *InAssetId.ToString());
    }
}

void AEnemyPawn::ApplySlow(float Ratio, float Duration)
{
    if (Status)
        Status->ApplySlow(Ratio, Duration);
}

void AEnemyPawn::ApplyStun(float Duration)
{
    if (Status)
        Status->ApplyStun(Duration);
}

void AEnemyPawn::HandleEnemyDeath()
{
    Destroy(); // 풀링 시스템과 연동 시 ReleaseEnemy로 교체 가능
}

void AEnemyPawn::SetIsActive(bool bNewActive)
{
    bIsActive = bNewActive;

    SetActorHiddenInGame(!bIsActive);
    SetActorEnableCollision(bIsActive);
    SetActorTickEnabled(bIsActive);

    ForceNetUpdate();
}

bool AEnemyPawn::GetIsActive() const
{
    return bIsActive;
}

void AEnemyPawn::SetEnemyData(UEnemyDataAsset* InData)
{
    EnemyData = InData;
    OnRep_EnemyData();
}

UEnemyDataAsset* AEnemyPawn::GetEnemyData() const
{
    return EnemyData;
}

bool AEnemyPawn::IsBoss() const
{
    return bIsBoss;
}

void AEnemyPawn::ReachGoal()
{
    if (AWaveManager* Manager = Cast<AWaveManager>(UGameplayStatics::GetActorOfClass(
        GetWorld(), AWaveManager::StaticClass())))
    {
        Manager->HandleEnemyReachedGoal(this);
    }
}
