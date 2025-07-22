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
    PrimaryActorTick.bCanEverTick = true;

    bReplicates = true;
    SetReplicateMovement(true);

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
    Mesh->SetRelativeRotation(MeshRotation);
    Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Mesh->SetCollisionResponseToAllChannels(ECR_Overlap);
    Mesh->SetGenerateOverlapEvents(true);

    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
    CollisionSphere->SetupAttachment(Mesh);
    CollisionSphere->SetSphereRadius(SphereRadius);
    CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionSphere->SetCollisionResponseToAllChannels(ECR_Overlap);
    CollisionSphere->SetGenerateOverlapEvents(true);

    DirectionIndicator = CreateDefaultSubobject<UArrowComponent>(TEXT("Direction"));
    DirectionIndicator->SetupAttachment(RootComponent);

    SplineMovement = CreateDefaultSubobject<UEnemySplineMovementComponent>(TEXT("SplineMovement"));
    Status = CreateDefaultSubobject<UEnemyStatusComponent>(TEXT("Status"));

    HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
    HealthBarWidget->SetupAttachment(Mesh);
    HealthBarWidget->SetWidgetSpace(EWidgetSpace::World);
    HealthBarWidget->SetDrawAtDesiredSize(true);
    HealthBarWidget->SetWidgetClass(EnemyHealthBarWidgetClass);

    Tags.Add(FName(TEXT("Enemy")));
    bIsActive = false;
    SetActorHiddenInGame(true);

    bGenerateOverlapEventsDuringLevelStreaming = true;
}

void AEnemyPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (HasAuthority() && SplineMovement)
    {
        DistanceAlongPath = SplineMovement->GetCurrentDistance();
    }
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

    if (EnemyData->Visual.Mesh)
    {
        Mesh->SetSkeletalMesh(EnemyData->Visual.Mesh);
    }

    if (EnemyData->Visual.AnimBPClass)
    {
        Mesh->SetAnimInstanceClass(EnemyData->Visual.AnimBPClass);
        Mesh->InitAnim(true);

        if (UEnemyAnimInstance* EnemyAnim = Cast<UEnemyAnimInstance>(Mesh->GetAnimInstance()))
        {
            EnemyAnim->SetSpeed(ReplicatedMoveSpeed);
        }
    }

    Mesh->SetRelativeScale3D(ReplicatedScale);
	CollisionSphere->SetRelativeScale3D(ReplicatedScale);
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

    if (EnemyData->Visual.Mesh)
    {
        Mesh->SetSkeletalMesh(EnemyData->Visual.Mesh);
    }

    if (EnemyData->Visual.AnimBPClass)
    {
        Mesh->SetAnimInstanceClass(EnemyData->Visual.AnimBPClass);
    }

    Mesh->SetRelativeScale3D(ReplicatedScale);

    if (Mesh && CollisionSphere)
    {
        FName CenterBoneName = TEXT("Body");

        if (Mesh->DoesSocketExist(CenterBoneName))
        {
            FVector BoneLocation = Mesh->GetSocketLocation(CenterBoneName);
            FVector LocalLocation = Mesh->GetComponentTransform().InverseTransformPosition(BoneLocation);
            CollisionSphere->SetRelativeLocation(LocalLocation);
        }
    }

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
    if (!HasAuthority()) return;

    if (!EnemyData) return;

    int32 RewardGold = EnemyData->Reward.Gold;

    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (APlayerController* PC = It->Get())
        {
            if (APolyPalsPlayerState* PS = PC->GetPlayerState<APolyPalsPlayerState>())
            {
                PS->AddGold(RewardGold);
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

    // 筌ｋ??계쳸???낅쑓??꾨뱜 ?곕떽?
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
    RewardGoldToPlayer();  // ?ⓥ뫀諭?筌왖疫?
    Destroy();             // ??筌띻낯?좑쭖?ReleaseEnemy嚥??대Ŋ猿?
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