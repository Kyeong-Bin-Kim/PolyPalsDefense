// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower/Components/TowerAttackComponent.h"
#include "Tower/PlacedTower.h"
#include "Multiplayer/PolyPalsController.h"
#include "Core/Subsystems/TowerDataManager.h"
#include "DataAsset/Tower/TowerPropertyData.h"
#include "DataAsset/Tower/TowerMaterialData.h"

#include "Net/UnrealNetwork.h"
#include "Components/SphereComponent.h"

UTowerAttackComponent::UTowerAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
}

void UTowerAttackComponent::BeginPlay()
{
	Super::BeginPlay();


	if (!GetOwner()->HasAuthority())
		GunMeshComponent->SetRelativeScale3D(FVector(2.f, 1.5f, 1.5f));

	OwnerTower->TowerRangeSphere->bHiddenInGame = false;
}


void UTowerAttackComponent::InitializeComponent()
{
	Super::InitializeComponent();

	OwnerTower = GetOwner<APlacedTower>();
	GunMeshComponent = OwnerTower->GunMeshComponent;
}

void UTowerAttackComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UTowerAttackComponent, CurrentLevel);
	DOREPLIFETIME(UTowerAttackComponent, TowerId);
}

void UTowerAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	//Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UTowerAttackComponent::ServerSetTowerIdByTower(uint8 InTowerId)
{
	TowerId = InTowerId;
	UTowerPropertyData* Data = GetWorld()->GetSubsystem<UTowerDataManager>()->GetTowerPropertyData(TowerId);
	FTowerUpgradeValue* UpgradeData = Data->UpgradeData.Find(ELevelValue::Level1);
	Damage = UpgradeData->Damage;
	AttackDelay = UpgradeData->AttackDelay;
	OwnerTower->TowerRangeSphere->SetSphereRadius(UpgradeData->Range);
}

void UTowerAttackComponent::ServerOnTowerLevelUp()
{
	if (!GetOwner()->HasAuthority()) return;
	if (TowerId <= 0) return;
	if (!GetWorld()) return;

	CurrentLevel++;
	UTowerPropertyData* Data = GetWorld()->GetSubsystem<UTowerDataManager>()->GetTowerPropertyData(TowerId);
	
	switch (CurrentLevel)
	{
	case 2:
		break;
	case 3:
		break;
	}

}

void UTowerAttackComponent::OnRep_CurrentLevel()
{
	switch (CurrentLevel)
	{
	case 2:
		GunMeshComponent->SetRelativeScale3D(FVector(3.5f, 1.5f, 1.5f));
		break;
	case 3:
		UMaterialInterface * TargetMaterial = GetWorld()->GetSubsystem<UTowerDataManager>()->GetTowerMaterialData()->GunMaxLevel;
		GunMeshComponent->SetMaterial(0, TargetMaterial);
		break;
	}
}

