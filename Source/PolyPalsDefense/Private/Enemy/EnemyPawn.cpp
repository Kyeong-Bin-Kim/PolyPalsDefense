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
    // ?ㅽ듃?뚰겕 蹂듭젣 ?ㅼ젙
    bReplicates = true;
    SetReplicateMovement(true);
    PrimaryActorTick.bCanEverTick = false;

    // 猷⑦듃 而댄룷?뚰듃
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    // ?ㅼ펷?덊깉 硫붿떆
    Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
    Mesh->SetRelativeRotation(MeshRotation);
    Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Mesh->SetCollisionResponseToAllChannels(ECR_Overlap);
    Mesh->SetGenerateOverlapEvents(true);

    // 硫붿떆 ?섏쐞??援ъ껜 肄쒕━??
    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
    CollisionSphere->SetupAttachment(Mesh);
    CollisionSphere->SetSphereRadius(SphereRadius);
    CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionSphere->SetCollisionResponseToAllChannels(ECR_Overlap);
    CollisionSphere->SetGenerateOverlapEvents(true);

    // 諛⑺뼢 ?쒖떆
    DirectionIndicator = CreateDefaultSubobject<UArrowComponent>(TEXT("Direction"));
    DirectionIndicator->SetupAttachment(RootComponent);

    // ?대룞 諛??곹깭 而댄룷?뚰듃
    SplineMovement = CreateDefaultSubobject<UEnemySplineMovementComponent>(TEXT("SplineMovement"));
    Status = CreateDefaultSubobject<UEnemyStatusComponent>(TEXT("Status"));

    // 泥대젰諛??쒖꽦
    HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
    HealthBarWidget->SetupAttachment(Mesh);
    HealthBarWidget->SetWidgetSpace(EWidgetSpace::World);
    HealthBarWidget->SetDrawAtDesiredSize(true);
    HealthBarWidget->SetWidgetClass(EnemyHealthBarWidgetClass); // ?대옒???ㅼ젙 ?꾩슂

    // ?쒓렇 諛?珥덇린 鍮꾪솢???곹깭
    Tags.Add(FName(TEXT("Enemy")));
    bIsActive = false;
    SetActorHiddenInGame(true);

    // ?ㅽ듃由щ컢 以묒뿉???ㅻ쾭???좎?
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

    // 硫붿떆 癒쇱? ?ㅼ젙
    if (EnemyData->Visual.Mesh)
    {
        Mesh->SetSkeletalMesh(EnemyData->Visual.Mesh);
        //UE_LOG(LogTemp, Warning, TEXT("[EnemyPawn] 硫붿떆 ?ㅼ젙?? %s"), *EnemyData->Visual.Mesh->GetName());
    }

    // ?좊땲硫붿씠??釉붾（?꾨┛???ㅼ젙
    if (EnemyData->Visual.AnimBPClass)
    {
        Mesh->SetAnimInstanceClass(EnemyData->Visual.AnimBPClass);
        //UE_LOG(LogTemp, Warning, TEXT("[EnemyPawn] 硫붿떆 ?ㅼ젙?? %s"), *EnemyData->Visual.Mesh->GetName());
        Mesh->InitAnim(true);

        // ?좊떂 ?몄뒪?댁뒪???띾룄 ?꾨떖
        if (UEnemyAnimInstance* EnemyAnim = Cast<UEnemyAnimInstance>(Mesh->GetAnimInstance()))
        {
            EnemyAnim->SetSpeed(ReplicatedMoveSpeed); // 蹂듭젣???띾룄 ?꾨떖
        }
    }

    // 蹂듭젣???ㅼ??쇰룄 ?곸슜
    Mesh->SetRelativeScale3D(ReplicatedScale);
	CollisionSphere->SetRelativeScale3D(ReplicatedScale);

    //UE_LOG(LogTemp, Warning, TEXT("[EnemyPawn] OnRep_EnemyData ?ㅽ뻾??- ?좊땲硫붿씠?? %s, ?ㅼ??? %s"),
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

    // 硫붿떆 ?ㅼ젙
    if (EnemyData->Visual.Mesh)
    {
        Mesh->SetSkeletalMesh(EnemyData->Visual.Mesh);
    }

    // ?좊땲硫붿씠??釉붾（?꾨┛???ㅼ젙
    if (EnemyData->Visual.AnimBPClass)
    {
        Mesh->SetAnimInstanceClass(EnemyData->Visual.AnimBPClass);
    }

    // ?ㅼ????곸슜
    Mesh->SetRelativeScale3D(ReplicatedScale);

    if (Mesh && CollisionSphere)
    {
        FName CenterBoneName = TEXT("Body");

        if (Mesh->DoesSocketExist(CenterBoneName))
        {
            FVector BoneLocation = Mesh->GetSocketLocation(CenterBoneName);
            FVector LocalLocation = Mesh->GetComponentTransform().InverseTransformPosition(BoneLocation);
            CollisionSphere->SetRelativeLocation(LocalLocation);

            //UE_LOG(LogTemp, Log, TEXT("[EnemyPawn] 以묒떖 蹂?湲곗? ?꾩튂 ?곸슜: %s"), *LocalLocation.ToString());
        }
    }

    // ?곹깭 而댄룷?뚰듃 珥덇린??
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
    if (!HasAuthority()) return; // 諛섎뱶???쒕쾭?먯꽌留??ㅽ뻾

    if (!EnemyData) return;

    int32 RewardGold = EnemyData->Reward.Gold;

    // ?쒕쾭?먯꽌 紐⑤뱺 PlayerController?먭쾶 怨⑤뱶 吏湲?
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (APlayerController* PC = It->Get())
        {
            if (APolyPalsPlayerState* PS = PC->GetPlayerState<APolyPalsPlayerState>())
            {
                PS->AddGold(RewardGold); // 怨⑤뱶 吏湲?
                UE_LOG(LogTemp, Log, TEXT("[EnemyPawn] %s?먭쾶 %d 怨⑤뱶 吏湲됰맖"), *PS->GetPlayerName(), RewardGold);
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

    // 泥대젰諛??낅뜲?댄듃 異붽?
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
    RewardGoldToPlayer();  // 怨⑤뱶 吏湲?
    Destroy();             // ?留곸씠硫?ReleaseEnemy濡?援먯껜
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