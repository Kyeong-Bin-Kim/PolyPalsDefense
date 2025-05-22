#include "EnemyPawn.h"
#include "Enemy/EnemyDataAsset.h"
#include "EnemySplineMovementComponent.h"
#include "EnemyStatusComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/ArrowComponent.h"
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

void AEnemyPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    // bIsActive 복제 등록
    DOREPLIFETIME(AEnemyPawn, bIsActive);
}

void AEnemyPawn::OnRep_IsActive()
{
    // 활성 상태에 따라 가시성 및 충돌, 틱 활성화/비활성화
    SetActorHiddenInGame(!bIsActive);
    SetActorEnableCollision(bIsActive);
    SetActorTickEnabled(bIsActive);
}

void AEnemyPawn::BeginPlay()
{
    Super::BeginPlay();
}

void AEnemyPawn::InitializeWithData(UEnemyDataAsset* InDataAsset, USplineComponent* InSpline, float HealthMultiplier, float SpeedMultiplier, FVector Scale)
{
    if (!InDataAsset) return;

    // 데이터 에셋 및 런타임 스탯 초기화
    EnemyData = InDataAsset;
    RuntimeStats = FEnemyRuntimeStats(EnemyData->Stats);
    if (bIsBoss)
        RuntimeStats.ApplyMultiplier(HealthMultiplier, SpeedMultiplier);

    // 메시, 애니메이션 세팅
    if (EnemyData->Visual.Mesh)
        Mesh->SetSkeletalMesh(EnemyData->Visual.Mesh);
    if (EnemyData->Visual.AnimBPClass)
        Mesh->SetAnimInstanceClass(EnemyData->Visual.AnimBPClass);

    // 크기 및 이동 속도 초기화
    Mesh->SetRelativeScale3D(Scale);
    BaseMoveSpeed = RuntimeStats.MoveSpeed;
    SplineMovement->Initialize(InSpline, BaseMoveSpeed);

    // 상태 컴포넌트 초기화 및 죽음 이벤트 바인딩
    if (Status)
    {
        Status->Initialize(RuntimeStats.MaxHealth);
        Status->OnEnemyDied.AddDynamic(this, &AEnemyPawn::HandleEnemyDeath);
    }
}

void AEnemyPawn::InitializeFromAssetId(const FPrimaryAssetId& AssetId, USplineComponent* InSpline, float HealthMultiplier, float SpeedMultiplier, FVector Scale)
{
    // 에셋 매니저로부터 데이터 에셋 로드
    UPrimaryDataAsset* Loaded = UPolyPalsDefenseAssetManager::Get().LoadPrimaryDataAsset(AssetId);
    if (UEnemyDataAsset* Casted = Cast<UEnemyDataAsset>(Loaded))
    {
        InitializeWithData(Casted, InSpline, HealthMultiplier, SpeedMultiplier, Scale);
    }
    else
    {
        // 에셋 로드 또는 캐스트 실패 시 에러 로그 출력
        UE_LOG(LogTemp, Error, TEXT("EnemyPawn: Asset loaded but cast failed: %s"), *AssetId.ToString());
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
    // 풀링 구조 도입 시에는 ReleaseEnemy 호출으로 대체 가능
    Destroy();
}

bool AEnemyPawn::IsBoss() const
{
    return bIsBoss;
}

void AEnemyPawn::ReachGoal()
{
    // 웨이브 매니저에게 골인 알림
    if (AWaveManager* Manager = Cast<AWaveManager>(UGameplayStatics::GetActorOfClass(
        GetWorld(), AWaveManager::StaticClass())))
    {
        Manager->HandleEnemyReachedGoal(this);
    }
}