#include "EnemyPawn.h"
#include "Enemy/EnemyDataAsset.h"
#include "EnemySplineMovementComponent.h"
#include "EnemyStatusComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/ArrowComponent.h"
#include "EnemyAnimInstance.h"
#include "AssetManagement/PolyPalsDefenseAssetManager.h"
#include "Kismet/GameplayStatics.h"
#include "UI/EnemyHealthBarWidget.h"
#include "WaveManager.h"
#include "Net/UnrealNetwork.h"
#include "PolyPalsPlayerState.h"


AEnemyPawn::AEnemyPawn()
{
    // 네트워크 복제 설정
    bReplicates = true;
    SetReplicateMovement(true);
    PrimaryActorTick.bCanEverTick = false;

    // 루트 컴포넌트
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    // 스켈레탈 메시
    Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
    Mesh->SetRelativeRotation(MeshRotation);
    Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Mesh->SetCollisionResponseToAllChannels(ECR_Overlap);
    Mesh->SetGenerateOverlapEvents(true);

    // 메시 하위에 구체 콜리전
    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
    CollisionSphere->SetupAttachment(Mesh);
    CollisionSphere->SetSphereRadius(SphereRadius);
    CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionSphere->SetCollisionResponseToAllChannels(ECR_Overlap);
    CollisionSphere->SetGenerateOverlapEvents(true);

    // 방향 표시
    DirectionIndicator = CreateDefaultSubobject<UArrowComponent>(TEXT("Direction"));
    DirectionIndicator->SetupAttachment(RootComponent);

    // 이동 및 상태 컴포넌트
    SplineMovement = CreateDefaultSubobject<UEnemySplineMovementComponent>(TEXT("SplineMovement"));
    Status = CreateDefaultSubobject<UEnemyStatusComponent>(TEXT("Status"));

    // 체력바 활성
    HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
    HealthBarWidget->SetupAttachment(Mesh);
    HealthBarWidget->SetWidgetSpace(EWidgetSpace::World);
    HealthBarWidget->SetDrawAtDesiredSize(true);
    HealthBarWidget->SetWidgetClass(EnemyHealthBarWidgetClass); // 클래스 설정 필요

    // 태그 및 초기 비활성 상태
    Tags.Add(FName(TEXT("Enemy")));
    bIsActive = false;
    SetActorHiddenInGame(true);

    // 스트리밍 중에도 오버랩 유지
    bGenerateOverlapEventsDuringLevelStreaming = true;
}



void AEnemyPawn::BeginPlay()
{
    Super::BeginPlay();

    
    Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Mesh->SetCollisionObjectType(ECC_Pawn);
    Mesh->SetCollisionResponseToAllChannels(ECR_Overlap);
    Tags.Add(FName(TEXT("Enemy")));
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
        //UE_LOG(LogTemp, Warning, TEXT("[EnemyPawn] 메시 설정됨: %s"), *EnemyData->Visual.Mesh->GetName());
    }

    // 애니메이션 블루프린트 설정
    if (EnemyData->Visual.AnimBPClass)
    {
        Mesh->SetAnimInstanceClass(EnemyData->Visual.AnimBPClass);
        //UE_LOG(LogTemp, Warning, TEXT("[EnemyPawn] 메시 설정됨: %s"), *EnemyData->Visual.Mesh->GetName());
        Mesh->InitAnim(true);

        // 애님 인스턴스에 속도 전달
        if (UEnemyAnimInstance* EnemyAnim = Cast<UEnemyAnimInstance>(Mesh->GetAnimInstance()))
        {
            EnemyAnim->SetSpeed(ReplicatedMoveSpeed); // 복제된 속도 전달
        }
    }

    // 복제된 스케일도 적용
    Mesh->SetRelativeScale3D(ReplicatedScale);
	CollisionSphere->SetRelativeScale3D(ReplicatedScale);

    //UE_LOG(LogTemp, Warning, TEXT("[EnemyPawn] OnRep_EnemyData 실행됨 - 애니메이션: %s, 스케일: %s"),
    //    EnemyData->Visual.AnimBPClass ? *EnemyData->Visual.AnimBPClass->GetName() : TEXT("None"),
    //    *ReplicatedScale.ToString());
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

    RuntimeStats = FEnemyRuntimeStats(EnemyData->Stats);
    if (bIsBoss)
    {
        RuntimeStats.ApplyMultiplier(HealthMultiplier, SpeedMultiplier);
    }

    ReplicatedMoveSpeed = RuntimeStats.MoveSpeed;

    // 메시 설정
    if (EnemyData->Visual.Mesh)
    {
        Mesh->SetSkeletalMesh(EnemyData->Visual.Mesh);
    }

    // 애니메이션 블루프린트 설정
    if (EnemyData->Visual.AnimBPClass)
    {
        Mesh->SetAnimInstanceClass(EnemyData->Visual.AnimBPClass);
    }

    // 스케일 적용
    Mesh->SetRelativeScale3D(ReplicatedScale);

    if (Mesh && CollisionSphere)
    {
        FName CenterBoneName = TEXT("Body");

        if (Mesh->DoesSocketExist(CenterBoneName))
        {
            FVector BoneLocation = Mesh->GetSocketLocation(CenterBoneName);
            FVector LocalLocation = Mesh->GetComponentTransform().InverseTransformPosition(BoneLocation);
            CollisionSphere->SetRelativeLocation(LocalLocation);

            //UE_LOG(LogTemp, Log, TEXT("[EnemyPawn] 중심 본 기준 위치 적용: %s"), *LocalLocation.ToString());
        }
    }

    // 상태 컴포넌트 초기화
    if (Status)
    {
        Status->Initialize(RuntimeStats.MaxHealth, RuntimeStats.MoveSpeed);
        Status->OnEnemyDied.RemoveDynamic(this, &AEnemyPawn::HandleEnemyDeath);
        Status->OnEnemyDied.AddDynamic(this, &AEnemyPawn::HandleEnemyDeath);
    }

    const float EffectiveSpeed = Status ? Status->GetEffectiveMoveSpeed() : RuntimeStats.MoveSpeed;
    SplineMovement->Initialize(InSpline, EffectiveSpeed);
}

void AEnemyPawn::RewardGoldToPlayer()
{
    if (!HasAuthority()) return; // 반드시 서버에서만 실행

    if (!EnemyData) return;

    int32 RewardGold = EnemyData->Reward.Gold;

    // 서버에서 모든 PlayerController에게 골드 지급
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (APlayerController* PC = It->Get())
        {
            if (APolyPalsPlayerState* PS = PC->GetPlayerState<APolyPalsPlayerState>())
            {
                PS->AddGold(RewardGold); // 골드 지급
                UE_LOG(LogTemp, Log, TEXT("[EnemyPawn] %s에게 %d 골드 지급됨"), *PS->GetPlayerName(), RewardGold);
            }
        }
    }
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

void AEnemyPawn::ReceiveDamage(float DamageAmount)
{
    if (Status)
    {
        Status->TakeDamage(DamageAmount);
    }

    // 체력바 업데이트 추가
    if (HealthBarWidget && HealthBarWidget->GetUserWidgetObject())
    {
        if (UEnemyHealthBarWidget* Widget = Cast<UEnemyHealthBarWidget>(HealthBarWidget->GetUserWidgetObject()))
        {
            float Ratio = GetHealth() / GetMaxHealth();
            Widget->SetHealthBarRatio(Ratio);
        }
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
    RewardGoldToPlayer();  // 골드 지급
    Destroy();             // 풀링이면 ReleaseEnemy로 교체
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

void AEnemyPawn::ReachGoal()
{
    if (AWaveManager* Manager = Cast<AWaveManager>(UGameplayStatics::GetActorOfClass(
        GetWorld(), AWaveManager::StaticClass())))
    {
        Manager->HandleEnemyReachedGoal(this);
    }
}

float AEnemyPawn::GetMaxHealth() const
{

    return Status ? Status->GetMaxHealth() : 0.f;
}

float AEnemyPawn::GetHealth() const
{
    return Status ? Status->GetCurrentHealth() : 0.f;
}

bool AEnemyPawn::IsSlowed() const
{
    return Status ? Status->IsSlowed() : false;
}

bool AEnemyPawn::IsStunned() const
{
    return Status ? Status->IsStunned() : false;
}

bool AEnemyPawn::IsBoss() const
{
    return bIsBoss;
}