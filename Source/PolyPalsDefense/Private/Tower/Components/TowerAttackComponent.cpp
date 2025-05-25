// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower/Components/TowerAttackComponent.h"
#include "Tower/PlacedTower.h"
#include "Multiplayer/PolyPalsController.h"
#include "Core/Subsystems/TowerDataManager.h"
#include "DataAsset/Tower/TowerPropertyData.h"
#include "DataAsset/Tower/TowerMaterialData.h"

#include "Enemy/EnemyPawn.h"

#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SphereComponent.h"
#include "Engine/StaticMeshSocket.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

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

	OwnerTower->TowerRangeSphere->SetHiddenInGame(false);
}


void UTowerAttackComponent::InitializeComponent()
{
	Super::InitializeComponent();

	OwnerTower = GetOwner<APlacedTower>();
	GunMeshComponent = OwnerTower->GunMeshComponent;
	MuzzleEffectComponent = OwnerTower->MuzzleEffectComponent;

	//MuzzleEffectComponent = OwnerTower->MuzzleEffectComponent;
}

void UTowerAttackComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UTowerAttackComponent, CurrentLevel);
	DOREPLIFETIME(UTowerAttackComponent, TowerId);
	DOREPLIFETIME(UTowerAttackComponent, CurrentTarget);
	DOREPLIFETIME(UTowerAttackComponent, bMuzzleEffect);
}

void UTowerAttackComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (GetWorld())
		GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void UTowerAttackComponent::ServerOnEnemyBeginOverlap(AActor* InEnemy)
{
	if (InEnemy)
	{
		SpottedEnemy_Server.AddUnique(InEnemy);

		if (TowerState_Server == ETowerState::Idle)
			SetTowerState(ETowerState::SpotTarget);
	}
}

void UTowerAttackComponent::ServerOnEnemyEndOverlap(AActor* InEnemy)
{
	SpottedEnemy_Server.Remove(InEnemy);

	if (CurrentTarget == InEnemy)
		CurrentTarget = nullptr;
}

void UTowerAttackComponent::ServerOnTowerAttack()
{
	if (IsValid(CurrentTarget))
	{
		if (bReadyToAttack)
		{
			UGameplayStatics::ApplyDamage(CurrentTarget, Damage, nullptr, nullptr, nullptr);
			DrawDebugSphere(GetWorld(), CurrentTarget->GetActorLocation(), 60.f, 12.f, FColor::Blue, false, 0.3f);
			bReadyToAttack = false;
			bMuzzleEffect = !bMuzzleEffect;

			if (GetWorld())
			{
				GetWorld()->GetTimerManager().SetTimer(DelayHandle, FTimerDelegate::CreateLambda([this]() {
					bReadyToAttack = true;
					ServerOnTowerAttack();
					}), AttackDelay, false);
			}
		}
	}
	else
		SetTowerState(ETowerState::LostTarget);
}

void UTowerAttackComponent::ClientUpdateGunMeshRotation()
{
	if (CurrentTarget)
	{
		FVector TargetLocation = CurrentTarget->GetActorLocation();
		FVector GunLocation = GunMeshComponent->GetComponentLocation();
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GunLocation, TargetLocation);

		FRotator NewRotation = FRotator(0.f, LookAtRotation.Yaw, 0.f);
		FRotator GunRotation = GunMeshComponent->GetComponentRotation();
		FRotator InterpedRotation = FMath::RInterpTo(GunRotation, NewRotation, GetWorld()->GetDeltaSeconds(), 1000.f);
		GunMeshComponent->SetRelativeRotation(InterpedRotation);
	}
}

void UTowerAttackComponent::ServerSetTowerIdByTower(uint8 InTowerId)
{
	TowerId = InTowerId;
	UTowerPropertyData* Data = GetWorld()->GetSubsystem<UTowerDataManager>()->GetTowerPropertyData(TowerId);
	FTowerUpgradeValue* UpgradeData = Data->UpgradeData.Find(ELevelValue::Level1);
	Damage = UpgradeData->Damage;
	AttackDelay = UpgradeData->AttackDelay;
	TowerAbility = Data->TowerAbility;
	/*MuzzleEffect = Data->MuzzleEffect;*/
	MuzzleEffectComponent->SetAsset(Data->MuzzleEffect);
	OwnerTower->TowerRangeSphere->SetSphereRadius(UpgradeData->Range);
}

void UTowerAttackComponent::SetGunMeshTimer()
{
	if (!GetWorld()) return;
	
	ClearTowerTimer(GunMeshHandle);
	GetWorld()->GetTimerManager().SetTimer(GunMeshHandle, this, &UTowerAttackComponent::ClientUpdateGunMeshRotation, 0.05f, true);
}

void UTowerAttackComponent::ClearTowerTimer(FTimerHandle& InHandle)
{
	if (!GetWorld()) return;

	if (GetWorld()->GetTimerManager().IsTimerActive(InHandle))
		GetWorld()->GetTimerManager().ClearTimer(InHandle);
}
void UTowerAttackComponent::SetTowerState(ETowerState InState)
{
	TowerState_Server = InState;
	switch (TowerState_Server)
	{
	case ETowerState::Idle:
		OnIdle();
		break;
	case ETowerState::SpotTarget:
		OnSpotTarget();
		break;
	case ETowerState::Attack:
		OnAttack();
		break;
	case ETowerState::LostTarget:
		OnLostTarget();
		break;
	}
}

void UTowerAttackComponent::OnIdle()
{
	// For extenstion method.
}

void UTowerAttackComponent::OnSpotTarget()
{
	for (const auto& Iter : SpottedEnemy_Server)
	{
		if (IsValid(Iter))
		{
			CurrentTarget = Iter;
			break;
		}
	}

	if (IsValid(CurrentTarget))
		SetTowerState(ETowerState::Attack);
	else
		SetTowerState(ETowerState::LostTarget);
}

void UTowerAttackComponent::OnAttack()
{
	ServerOnTowerAttack();
}

void UTowerAttackComponent::OnLostTarget()
{
	SpottedEnemy_Server.RemoveAll([](AActor* Candidate) {
		return !IsValid(Candidate);
		});

	if (SpottedEnemy_Server.IsEmpty())
		SetTowerState(ETowerState::Idle);
	else
		SetTowerState(ETowerState::SpotTarget);
}

void UTowerAttackComponent::OnRep_MuzzleEffect()
{
	MuzzleEffectComponent->ActivateSystem();
}

void UTowerAttackComponent::OnRep_TowerId()
{
	if (GetWorld())
	{
		UTowerPropertyData* TowerData = GetWorld()->GetSubsystem<UTowerDataManager>()->GetTowerPropertyData(TowerId);
		FTowerUpgradeValue* UpgradeData = TowerData->UpgradeData.Find(ELevelValue::Level1);
		AttackDelay = UpgradeData->AttackDelay;
	}
}

void UTowerAttackComponent::OnRep_CurrentTarget()
{
	if (IsValid(CurrentTarget))
		SetGunMeshTimer();
	else
		ClearTowerTimer(GunMeshHandle);
}

void UTowerAttackComponent::ServerOnTowerLevelUp()
{
	if (!GetOwner()->HasAuthority()) return;
	if (TowerId <= 0) return;
	if (!GetWorld()) return;

	CurrentLevel++;
	UTowerPropertyData* Data = GetWorld()->GetSubsystem<UTowerDataManager>()->GetTowerPropertyData(TowerId);
	
	FTowerUpgradeValue* UpgradeData = Data->UpgradeData.Find(ELevelValue::Level1);

	switch (CurrentLevel)
	{
	case 2:
		UpgradeData = Data->UpgradeData.Find(ELevelValue::Level2);
		break;
	case 3:
		UpgradeData = Data->UpgradeData.Find(ELevelValue::Level3);
		break;
	}

	Damage = UpgradeData->Damage;
	AttackDelay = UpgradeData->AttackDelay;
	OwnerTower->TowerRangeSphere->SetSphereRadius(UpgradeData->Range);
}

void UTowerAttackComponent::OnRep_CurrentLevel()
{
	if (!GetWorld()) return;
	if (TowerId <= 0) return;

	UTowerDataManager* DataManager = GetWorld()->GetSubsystem<UTowerDataManager>();
	UTowerPropertyData* PropertyData = DataManager->GetTowerPropertyData(TowerId);
	FTowerUpgradeValue* TowerUpgradeValue = PropertyData->UpgradeData.Find(static_cast<ELevelValue>(CurrentLevel));
	AttackDelay = TowerUpgradeValue->AttackDelay;
	//SetAttackTimer();

	switch (CurrentLevel)
	{
	case 2:
		GunMeshComponent->SetRelativeScale3D(FVector(3.5f, 1.5f, 1.5f));
		break;
	case 3:
		UMaterialInterface * TargetMaterial = DataManager->GetTowerMaterialData()->GunMaxLevel;
		GunMeshComponent->SetMaterial(0, TargetMaterial);
		break;
	}
}

