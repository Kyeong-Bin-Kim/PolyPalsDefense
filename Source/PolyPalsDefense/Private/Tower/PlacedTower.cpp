// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower/PlacedTower.h"
#include "Tower/Components/TowerAttackComponent.h"
#include "Tower/Components/TowerUpWidgetComponent.h"
#include "Multiplayer/PolyPalsController.h"
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

// Sets default values
APlacedTower::APlacedTower()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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

// Called when the game starts or when spawned
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
		TowerUpWidgetComponent->Deactivate();
	else
		ClientSetupTowerWidgetComponent();

	UpdateLevelText();
	//test code
	//if (HasAuthority())
	//{
	//	FTimerHandle handle;
	//	GetWorld()->GetTimerManager().SetTimer(handle, FTimerDelegate::CreateLambda([this]() {

	//		if (TowerId > 0)
	//			ExternalInitializeTower(TowerId, EPlayerColor::None);

	//		}), 1.f, false);
	//}
}

void APlacedTower::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlacedTower, OwnerController);
	DOREPLIFETIME(APlacedTower, TowerId);
	DOREPLIFETIME(APlacedTower, PlayerColor);
}

// Called every frame
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
	TowerId = InTowerId;
	PlayerColor = InColor;
	if (TowerAttackComponent)
	{
		TowerAttackComponent->ServerSetTowerIdByTower(TowerId);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("TowerAttackComponent is null in ExternalInitializeTower!"));
	}

	if (IsValid(InController))
	{
		SetOwner(InController);
		OwnerController = InController;
	}
	// test code
	//OnRep_TowerId();
	//OnRep_PlayerColor();
}

void APlacedTower::OnRep_PlayerColor()
{
	if (!GetWorld()) return;

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
	if (!GetWorld()) return;

	if (TowerId > 0)
	{
		UTowerPropertyData* PropertyData = GetWorld()->GetSubsystem<UTowerDataManager>()->GetTowerPropertyData(TowerId);
		TowerMeshComponent->SetStaticMesh(PropertyData->TowerMesh);
		GunMeshComponent->SetStaticMesh(PropertyData->GunMesh);
		MuzzleEffectComponent->SetAsset(PropertyData->MuzzleEffect);
	}
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
	//if (!HasAuthority()) return; // 서버에서만 처리

	if (Level >= MaxLevel)
	{
		UE_LOG(LogTemp, Warning, TEXT("Upgrade failed: Already max level."));
		return;
	}

	APolyPalsController* PC = Cast<APolyPalsController>(GetOwner()); // 더 안전한 방식
	APolyPalsPlayerState* PS = PC ? Cast<APolyPalsPlayerState>(PC->PlayerState) : nullptr;

	if (PS && PS->GetPlayerGold() >= UpgradeCost)
	{
		// 골드 차감
		PS->AddGold(-UpgradeCost);

		// 레벨 증가
		Level++;

		// 외형 변경 및 레벨 텍스트 갱신
		UpdateTowerAppearance();
		UpdateLevelText();

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


void APlacedTower::UpdateTowerAppearance()
{
}

void APlacedTower::UpdateLevelText()
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

void APlacedTower::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//FString Overlapped = OtherActor->GetName();
	//UE_LOG(LogTemp, Log, TEXT("Tower range overlapped with %s"), *Overlapped);

	if (OtherActor->ActorHasTag(AttackTargetTag))
	{
		//FString MyName = GetName();
		//FString EneymName = OtherActor->GetName();
		//UE_LOG(LogTemp, Log, TEXT("Tower %s Spotted Enemy %s"), *MyName, *EneymName);
		TowerAttackComponent->ServerOnEnemyBeginOverlap(OtherActor);
	}
}

void APlacedTower::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag(AttackTargetTag))
	{
		//FString MyName = GetName();
		//FString EneymName = OtherActor->GetName();
		//UE_LOG(LogTemp, Log, TEXT("Tower %s lost Enemy %s"), *MyName, *EneymName);
		TowerAttackComponent->ServerOnEnemyEndOverlap(OtherActor);
	}
}

