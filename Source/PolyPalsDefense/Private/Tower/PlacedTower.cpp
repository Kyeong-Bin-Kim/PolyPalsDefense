// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower/PlacedTower.h"
#include "Tower/Components/TowerAttackComponent.h"
#include "Core/Subsystems/TowerDataManager.h"
#include "DataAsset/Tower/TowerMaterialData.h"
#include "DataAsset/Tower/TowerPropertyData.h"

#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
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

	TowerRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TowerRangeSphere"));
	TowerRangeSphere->SetupAttachment(RootBoxComponent);

	TowerAttackComponent = CreateDefaultSubobject<UTowerAttackComponent>(TEXT("TowerAttackComponent"));
	TowerAttackComponent->SetIsReplicated(true);
	SetTowerCollision();
	Tags.Add(FName("Tower"));
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

void APlacedTower::ExternalInitializeTower(uint8 InTowerId, EPlayerColor InColor)
{
	TowerId = InTowerId;
	PlayerColor = InColor;
	TowerAttackComponent->ServerSetTowerIdByTower(TowerId);

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

void APlacedTower::SetTowerCollision()
{
	TowerMeshComponent->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel2);
	TowerMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TowerMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);
}

void APlacedTower::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FString Overlapped = OtherActor->GetName();
	UE_LOG(LogTemp, Log, TEXT("Tower range overlapped with %s"), *Overlapped);

	if (OtherActor->ActorHasTag(AttackTargetTag))
	{
		FString MyName = GetName();
		FString EneymName = OtherActor->GetName();
		UE_LOG(LogTemp, Log, TEXT("Tower %s Spotted Enemy %s"), *MyName, *EneymName);
		TowerAttackComponent->ServerOnEnemyBeginOverlap(OtherActor);
	}
}

void APlacedTower::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag(AttackTargetTag))
	{
		FString MyName = GetName();
		FString EneymName = OtherActor->GetName();
		UE_LOG(LogTemp, Log, TEXT("Tower %s lost Enemy %s"), *MyName, *EneymName);
		TowerAttackComponent->ServerOnEnemyEndOverlap(OtherActor);
	}
}

