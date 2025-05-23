// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower/Components/TowerAttackComponent.h"
#include "Tower/PlacedTower.h"
#include "Tower/TowerAttackInterface.h"
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
#include "NiagaraFunctionLibrary.h"

UTowerAttackComponent::UTowerAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
}

void UTowerAttackComponent::BeginPlay()
{
	Super::BeginPlay();


	if (!GetOwner()->HasAuthority())
		GunMeshComponent->SetRelativeScale3D(FVector(2.f, 1.5f, 1.5f));

	OwnerTower->TowerRangeSphere->SetHiddenInGame(false);
	SetComponentTickEnabled(false);
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
	DOREPLIFETIME(UTowerAttackComponent, CurrentTarget);
}

void UTowerAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CurrentTarget)
	{
		FVector TargetLocatoin = CurrentTarget->GetActorLocation();
		FVector GunLocation = GunMeshComponent->GetComponentLocation();
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GunLocation, TargetLocatoin);

		FRotator NewRotation = FRotator(0.f, LookAtRotation.Yaw, 0.f);
		FRotator GunRotatoin = GunMeshComponent->GetComponentRotation();
		FRotator InterpedRotation = FMath::RInterpTo(GunRotatoin, NewRotation, DeltaTime, 1000.f);
		GunMeshComponent->SetWorldRotation(InterpedRotation);
		//GunMeshComponent->SetWorldRotation(NewRotation);
	}
}

void UTowerAttackComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

}

void UTowerAttackComponent::ServerOnEnemyBeginOverlap(AActor* InEnemy)
{
	if (InEnemy)
	{
		SpottedEnemy_Server.AddUnique(InEnemy);

		if (!SpottedEnemy_Server.IsEmpty())
		{
			if (CurrentTarget != SpottedEnemy_Server[0])
				CurrentTarget = SpottedEnemy_Server[0];

			if (!GetWorld()->GetTimerManager().IsTimerActive(AttackHandle))
				SetAttackTimer();
		}
	}
}

void UTowerAttackComponent::ServerOnEnemyEndOverlap(AActor* InEnemy)
{
	SpottedEnemy_Server.Remove(InEnemy);

	if (SpottedEnemy_Server.IsEmpty())
		CurrentTarget = nullptr;
}

AActor* UTowerAttackComponent::ServerFindFirstValidTarget()
{
	SpottedEnemy_Server.RemoveAll([](AActor* Candidate) {
		return !IsValid(Candidate);
		});

	return SpottedEnemy_Server.IsValidIndex(0) ? SpottedEnemy_Server[0] : nullptr;
}

void UTowerAttackComponent::ServerOnTowerAttack()
{
	AActor* Target = ServerFindFirstValidTarget();

	if (!IsValid(Target))
	{
		CurrentTarget = nullptr;
		ClearAttackTimer();
		return;
	}

	CurrentTarget = Target;
	UGameplayStatics::ApplyDamage(Target, Damage, nullptr, nullptr, nullptr);

	AEnemyPawn* EnemyPawn = Cast<AEnemyPawn>(Target);
	if (IsValid(EnemyPawn))	// 캐스트가 성공했는가? 혹은 다른 이유로 Destroy 되지 않았는가?
	{
		switch (TowerAbility)
		{
		case ETowerAbility::Slow:
			EnemyPawn->ApplySlow(0.5f, 2.f);
			break;
		case ETowerAbility::Stun:
			EnemyPawn->ApplyStun(2.f);
			break;
		}
	}
}

void UTowerAttackComponent::ClientOnTowerAttack()
{
	UNiagaraFunctionLibrary::SpawnSystemAttached(MuzzleEffect,
		GunMeshComponent, FName("MuzzleSocket"),
		FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepWorldPosition,
		true, true, ENCPoolMethod::AutoRelease, true);
}

void UTowerAttackComponent::ServerSetTowerIdByTower(uint8 InTowerId)
{
	TowerId = InTowerId;
	UTowerPropertyData* Data = GetWorld()->GetSubsystem<UTowerDataManager>()->GetTowerPropertyData(TowerId);
	FTowerUpgradeValue* UpgradeData = Data->UpgradeData.Find(ELevelValue::Level1);
	Damage = UpgradeData->Damage;
	AttackDelay = UpgradeData->AttackDelay;
	TowerAbility = Data->TowerAbility;
	OwnerTower->TowerRangeSphere->SetSphereRadius(UpgradeData->Range);
}

void UTowerAttackComponent::SetAttackTimer()
{
	ClearAttackTimer();

	if (GetOwner()->HasAuthority())
		GetWorld()->GetTimerManager().SetTimer(AttackHandle, this, &UTowerAttackComponent::ServerOnTowerAttack, AttackDelay, true);
	else
		GetWorld()->GetTimerManager().SetTimer(AttackHandle, this, &UTowerAttackComponent::ClientOnTowerAttack, AttackDelay, true);
}

void UTowerAttackComponent::ClearAttackTimer()
{
	if (!GetWorld()) return;

	if (GetWorld()->GetTimerManager().IsTimerActive(AttackHandle))
		GetWorld()->GetTimerManager().ClearTimer(AttackHandle);
}

void UTowerAttackComponent::OnRep_TowerId()
{
	if (GetWorld())
	{
		UTowerPropertyData* TowerData = GetWorld()->GetSubsystem<UTowerDataManager>()->GetTowerPropertyData(TowerId);
		FTowerUpgradeValue* UpgradeData = TowerData->UpgradeData.Find(ELevelValue::Level1);
		AttackDelay = UpgradeData->AttackDelay;
		MuzzleEffect = TowerData->MuzzleEffect;
	}
}

void UTowerAttackComponent::OnRep_CurrentTarget()
{
	if (CurrentTarget)
	{
		SetAttackTimer();
		SetComponentTickEnabled(true);
	}
	else
	{
		ClearAttackTimer();
		SetComponentTickEnabled(false);
	}
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
	SetAttackTimer();

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

