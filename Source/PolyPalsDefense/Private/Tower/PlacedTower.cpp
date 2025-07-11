#include "Tower/PlacedTower.h"
#include "Tower/Components/TowerAttackComponent.h"
#include "Tower/Components/TowerUpWidgetComponent.h"
#include "Core/Subsystems/TowerDataManager.h"
#include "DataAsset/Tower/TowerMaterialData.h"
#include "DataAsset/Tower/TowerPropertyData.h"
#include "UI/TowerUpgradeWidget.h"

#include "Tower/PreviewBuilding.h"
#include "UI/TowerLevelWidget.h"
#include "Components/WidgetComponent.h"
#include "UI/PolyPalsHUD.h"
#include "PolyPalsPlayerState.h"
#include "PolyPalsController.h"

#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraComponentPoolMethodEnum.h"
#include "Net/UnrealNetwork.h"

APlacedTower::APlacedTower()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	RootBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("RootBoxComponent"));
	SetRootComponent(RootBoxComponent);
	RootBoxComponent->SetBoxExtent(FVector(32.f, 32.f, 80.f));

	TowerMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TowerMeshComponent"));
	TowerMeshComponent->SetupAttachment(RootBoxComponent);
	TowerMeshComponent->SetRelativeLocation(FVector(0.f, 0.f, -80.f));
	ConstructorHelpers::FObjectFinder<UStaticMesh> TowerMesh(TEXT("/Game/Meshs/Static/AssembledTower/SM_NullTower.SM_NullTower"));
	
	if (TowerMesh.Succeeded())
		TowerMeshComponent->SetStaticMesh(TowerMesh.Object);
	TowerMeshComponent->SetRelativeScale3D(FVector(0.85f, 0.85f, 0.85f));

	GunMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GunMeshComponent"));
	GunMeshComponent->SetupAttachment(TowerMeshComponent, FName("GunAttachPoint"));

	TowerUpWidgetComponent = CreateDefaultSubobject<UTowerUpWidgetComponent>(TEXT("TowerUpWidgetComponent"));
	TowerUpWidgetComponent->SetupAttachment(RootBoxComponent);
	ConstructorHelpers::FClassFinder<UTowerUpgradeWidget> UpWidgetClass(TEXT("/Game/UI/WBP_TowerUpgrade.WBP_TowerUpgrade_C"));
	
	if (UpWidgetClass.Succeeded())
		TowerUpWidgetClass = UpWidgetClass.Class;

	MuzzleEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("MuzzleEffectComponent"));
	MuzzleEffectComponent->SetupAttachment(GunMeshComponent, FName("MuzzleSocket"));
	MuzzleEffectComponent->bAutoActivate = false;

	TowerRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TowerRangeSphere"));
	TowerRangeSphere->SetupAttachment(RootBoxComponent);

	TowerAttackComponent = CreateDefaultSubobject<UTowerAttackComponent>(TEXT("TowerAttackComponent"));
	TowerAttackComponent->SetIsReplicated(true);
	SetTowerCollision();
	Tags.Add(FName("Tower"));

	LevelWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("LevelWidgetComponent"));
	LevelWidgetComponent->SetupAttachment(RootComponent);
	LevelWidgetComponent->SetRelativeLocation(FVector(-10.f, 10.f, 150.f)); // 타워 아래쪽
	LevelWidgetComponent->SetRelativeRotation(FRotator(90.f, 180.f, 0.f)); // 타워 위쪽을 바라보도록 회전
	LevelWidgetComponent->SetWidgetSpace(EWidgetSpace::World); // 또는 World
	LevelWidgetComponent->SetDrawSize(FVector2D(128.f, 64.f)); // 크기 조절
	LevelWidgetComponent->SetVisibility(true); // 초기엔 숨겨두기

}

void APlacedTower::BeginPlay()
{
	Super::BeginPlay();
	
	SetTowerCollision();
	GunMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TowerRangeSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
	TowerRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &APlacedTower::OnBeginOverlap);
	TowerRangeSphere->OnComponentEndOverlap.AddDynamic(this, &APlacedTower::OnEndOverlap);


	TowerUpWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (HasAuthority())
	{
		TowerUpWidgetComponent->Deactivate();
	}
}

void APlacedTower::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlacedTower, OwnerController);
	DOREPLIFETIME(APlacedTower, TowerId);
	DOREPLIFETIME(APlacedTower, PlayerColor);
}

void APlacedTower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlacedTower::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	}
}

void APlacedTower::ExternalInitializeTower(uint8 InTowerId, EPlayerColor InColor, APolyPalsController* const InController)
{
	if (!HasAuthority()) return;

	TowerId = InTowerId;
	PlayerColor = InColor;

	if (TowerAttackComponent)
	{
		TowerAttackComponent->ServerSetTowerIdByTower(TowerId);
	}

	if (IsValid(InController))
	{
		SetOwner(InController);
		OwnerController = InController;
	}

	Multicast_InitializeVisuals(InTowerId, InColor);
}

void APlacedTower::Multicast_InitializeVisuals_Implementation(uint8 InTowerId, EPlayerColor InColor)
{
	TowerId = InTowerId;
	PlayerColor = InColor;

	SetupVisuals();
	ClientSetupTowerWidgetComponent();
}

void APlacedTower::Multicast_UpdateLevelVisuals_Implementation(uint8 InTowerId, EPlayerColor InColor, int32 InNewLevel)
{
	// 모든 클라이언트에서 실행
	TowerId = InTowerId;
	PlayerColor = InColor;
	Level = InNewLevel;

	SetupVisuals();   // 메시,머티어얼 업데이트
	UpdateLevel();    // 레벨, 이펙트 등 업데이트
}

void APlacedTower::OnRep_PlayerColor()
{
	if (!GetWorld()) return;

	UE_LOG(LogTemp, Warning, TEXT("[APlacedTower] OnRep_PlayerColor fired: TowerId=%d"), TowerId);

	if (TowerId > 0)
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(VisualSetupHandle))
			GetWorld()->GetTimerManager().ClearTimer(VisualSetupHandle);

		ClientSetTowerMeshComponent(TowerId, PlayerColor);
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(VisualSetupHandle, FTimerDelegate::CreateLambda([this]() {
			OnRep_PlayerColor();
			}), 0.1f, true);
	}
}

void APlacedTower::OnRep_TowerId()
{
	SetupVisuals();
}

void APlacedTower::ClientSetupTowerWidgetComponent()
{
	TowerUpWidgetComponent->SetWidgetClass(TowerUpWidgetClass);
	TowerUpWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
	TowerUpWidgetComponent->SetDrawSize(FVector2D(85.f, 50.f));
	TowerUpWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	TowerUpWidgetComponent->SetHiddenInGame(true);

	if (UTowerUpgradeWidget* Widget = Cast<UTowerUpgradeWidget>(TowerUpWidgetComponent->GetWidget()))
	{
		Widget->SetTargetTower(this); 
	}
}

void APlacedTower::ClientSetTowerMeshComponent(uint8 InTowerId, EPlayerColor InColor)
{
	if (!GetWorld()) return;

	UE_LOG(LogTemp, Warning, TEXT("[APlacedTower] ClientSetTowerMeshComponent called: InTowerId=%d, InColor=%d"), InTowerId, static_cast<int32>(InColor));
	
	UTowerMaterialData* MaterialData = GetWorld()->GetSubsystem<UTowerDataManager>()->GetTowerMaterialData();
	UMaterialInterface* TargetMaterial = nullptr;

	switch (InColor)
	{
	case EPlayerColor::Red:
		TargetMaterial = MaterialData->PlayerRed;
		break;
	case EPlayerColor::Blue:
		TargetMaterial = MaterialData->PlayerBlue;
		break;
	case EPlayerColor::Green:
		TargetMaterial = MaterialData->PlayerGreen;
		break;
	case EPlayerColor::Yellow:
		TargetMaterial = MaterialData->PlayerYellow;
		break;
	default:
		TargetMaterial = MaterialData->Default;
		break;
	}

	int32 MatSlotCount = TowerMeshComponent->GetNumMaterials();

	for (uint8 Iter = 0; Iter < MatSlotCount; Iter++)
	{
		TowerMeshComponent->SetMaterial(Iter, TargetMaterial);
	}
}

void APlacedTower::SetWidgetHidden(bool bIsDeactice)
{
	TowerUpWidgetComponent->SetHiddenInGame(bIsDeactice);
}

void APlacedTower::UpgradeTower()
{
    if (!HasAuthority())
    {
        Server_RequestUpgradeTower();
        return;
    }
}

void APlacedTower::Server_RequestUpgradeTower_Implementation()
{
	if (Level >= MaxLevel)
	{
		UE_LOG(LogTemp, Warning, TEXT("Upgrade failed: Already max level."));
		return;
	}

	APolyPalsController* PC = Cast<APolyPalsController>(GetOwner());
	APolyPalsPlayerState* PS = PC ? Cast<APolyPalsPlayerState>(PC->PlayerState) : nullptr;

	if (PS && PS->GetPlayerGold() >= UpgradeCost)
	{
		// 골드 차감
		PS->AddGold(-UpgradeCost);

		// 레벨 증가
		Level++;

		// 외형 변경 및 레벨 갱신
		Multicast_UpdateLevelVisuals(TowerId, PlayerColor, Level);

		// 로컬 클라이언트라면 HUD도 갱신
		if (PC->IsLocalController())
		{
			if (APolyPalsHUD* HUD = Cast<APolyPalsHUD>(PC->GetHUD()))
			{
				HUD->UpdateGoldOnUI(PS->GetPlayerGold());
			}
		}

		UE_LOG(LogTemp, Log, TEXT("Tower upgraded! New Level: %d"), Level);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Upgrade failed: Not enough gold."));
	}
}

void APlacedTower::UpdateLevel()
{
	if (!LevelWidgetComponent) return;

	UTowerLevelWidget* LevelWidget = Cast<UTowerLevelWidget>(LevelWidgetComponent->GetUserWidgetObject());

	if (LevelWidget)
	{
		LevelWidget->UpdateLevelUI(Level); // 현재 타워 레벨 전달
	}
}

void APlacedTower::SetTowerCollision()
{
	TowerMeshComponent->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel2);
	TowerMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	TowerMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);
	TowerMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Block);
}

void APlacedTower::SetupVisuals()
{
	if (TowerId < 0) return;

	auto* PD = GetWorld()->GetSubsystem<UTowerDataManager>()->GetTowerPropertyData(TowerId);
	
	if (!PD)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetupVisuals: Invalid TowerId=%d"), TowerId);
		return;
	}

	// 타워, 총, 이펙트 설정
	TowerMeshComponent->SetStaticMesh(PD->TowerMesh);
	GunMeshComponent->SetStaticMesh(PD->GunMesh);
	MuzzleEffectComponent->SetAsset(PD->MuzzleEffect);

	// 플레이어 컬러(Material)
	ClientSetTowerMeshComponent(TowerId, PlayerColor);
}

void APlacedTower::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag(AttackTargetTag))
	{
		TowerAttackComponent->ServerOnEnemyBeginOverlap(OtherActor);
	}
}

void APlacedTower::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag(AttackTargetTag))
	{
		TowerAttackComponent->ServerOnEnemyEndOverlap(OtherActor);
	}
}

