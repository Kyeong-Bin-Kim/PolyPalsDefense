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

    // ??쎈뱜??곌쾿 癰귣벊????쇱젟
    bReplicates = true;
    SetReplicateMovement(true);

    // ?룐뫂???뚮똾猷??곕뱜
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    // ??쇳렩??딄퉱 筌롫뗄??
    Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
    Mesh->SetRelativeRotation(MeshRotation);
    Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Mesh->SetCollisionResponseToAllChannels(ECR_Overlap);
    Mesh->SetGenerateOverlapEvents(true);

    // 筌롫뗄????륁맄???닌딄퍥 ?꾩뮆???
    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
    CollisionSphere->SetupAttachment(Mesh);
    CollisionSphere->SetSphereRadius(SphereRadius);
    CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionSphere->SetCollisionResponseToAllChannels(ECR_Overlap);
    CollisionSphere->SetGenerateOverlapEvents(true);

    // 獄쎻뫚堉???뽯뻻
    DirectionIndicator = CreateDefaultSubobject<UArrowComponent>(TEXT("Direction"));
    DirectionIndicator->SetupAttachment(RootComponent);

    // ??猷?獄??怨밴묶 ?뚮똾猷??곕뱜
    SplineMovement = CreateDefaultSubobject<UEnemySplineMovementComponent>(TEXT("SplineMovement"));
    Status = CreateDefaultSubobject<UEnemyStatusComponent>(TEXT("Status"));

    // 筌ｋ??계쳸???뽮쉐
    HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
    HealthBarWidget->SetupAttachment(Mesh);
    HealthBarWidget->SetWidgetSpace(EWidgetSpace::World);
    HealthBarWidget->SetDrawAtDesiredSize(true);
    HealthBarWidget->SetWidgetClass(EnemyHealthBarWidgetClass); // ???????쇱젟 ?袁⑹뒄

    // ??볥젃 獄??λ뜃由???쑵????怨밴묶
    Tags.Add(FName(TEXT("Enemy")));
    bIsActive = false;
    SetActorHiddenInGame(true);

    // ??쎈뱜?귐됱빪 餓λ쵐肉????살쒔???醫?
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

    // 筌롫뗄???믪눘? ??쇱젟
    if (EnemyData->Visual.Mesh)
    {
        Mesh->SetSkeletalMesh(EnemyData->Visual.Mesh);
        //UE_LOG(LogTemp, Warning, TEXT("[EnemyPawn] 筌롫뗄????쇱젟?? %s"), *EnemyData->Visual.Mesh->GetName());
    }

    // ?醫딅빍筌롫뗄????됰뗀竊?袁ⓥ뵛????쇱젟
    if (EnemyData->Visual.AnimBPClass)
    {
        Mesh->SetAnimInstanceClass(EnemyData->Visual.AnimBPClass);
        //UE_LOG(LogTemp, Warning, TEXT("[EnemyPawn] 筌롫뗄????쇱젟?? %s"), *EnemyData->Visual.Mesh->GetName());
        Mesh->InitAnim(true);

        // ?醫딅뻷 ?紐꾨뮞??곷뮞????얜즲 ?袁⑤뼎
        if (UEnemyAnimInstance* EnemyAnim = Cast<UEnemyAnimInstance>(Mesh->GetAnimInstance()))
        {
            EnemyAnim->SetSpeed(ReplicatedMoveSpeed); // 癰귣벊?????얜즲 ?袁⑤뼎
        }
    }

    // 癰귣벊????????곕즲 ?怨몄뒠
    Mesh->SetRelativeScale3D(ReplicatedScale);
	CollisionSphere->SetRelativeScale3D(ReplicatedScale);

    //UE_LOG(LogTemp, Warning, TEXT("[EnemyPawn] OnRep_EnemyData ??쎈뻬??- ?醫딅빍筌롫뗄??? %s, ????? %s"),
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

    // 筌롫뗄????쇱젟
    if (EnemyData->Visual.Mesh)
    {
        Mesh->SetSkeletalMesh(EnemyData->Visual.Mesh);
    }

    // ?醫딅빍筌롫뗄????됰뗀竊?袁ⓥ뵛????쇱젟
    if (EnemyData->Visual.AnimBPClass)
    {
        Mesh->SetAnimInstanceClass(EnemyData->Visual.AnimBPClass);
    }

    // ??????怨몄뒠
    Mesh->SetRelativeScale3D(ReplicatedScale);

    if (Mesh && CollisionSphere)
    {
        FName CenterBoneName = TEXT("Body");

        if (Mesh->DoesSocketExist(CenterBoneName))
        {
            FVector BoneLocation = Mesh->GetSocketLocation(CenterBoneName);
            FVector LocalLocation = Mesh->GetComponentTransform().InverseTransformPosition(BoneLocation);
            CollisionSphere->SetRelativeLocation(LocalLocation);

            //UE_LOG(LogTemp, Log, TEXT("[EnemyPawn] 餓λ쵐??癰?疫꿸퀣? ?袁⑺뒄 ?怨몄뒠: %s"), *LocalLocation.ToString());
        }
    }

    // ?怨밴묶 ?뚮똾猷??곕뱜 ?λ뜃由??
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
    if (!HasAuthority()) return; // 獄쏆꼶諭????뺤쒔?癒?퐣筌???쎈뻬

    if (!EnemyData) return;

    int32 RewardGold = EnemyData->Reward.Gold;

    // ??뺤쒔?癒?퐣 筌뤴뫀諭?PlayerController?癒?쓺 ?ⓥ뫀諭?筌왖疫?
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (APlayerController* PC = It->Get())
        {
            if (APolyPalsPlayerState* PS = PC->GetPlayerState<APolyPalsPlayerState>())
            {
                PS->AddGold(RewardGold); // ?ⓥ뫀諭?筌왖疫?
                UE_LOG(LogTemp, Log, TEXT("[EnemyPawn] %s?癒?쓺 %d ?ⓥ뫀諭?筌왖疫뀀맧留?), *PS->GetPlayerName(), RewardGold);
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