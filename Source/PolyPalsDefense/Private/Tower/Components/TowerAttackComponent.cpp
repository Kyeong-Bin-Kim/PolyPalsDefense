// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/TowerAttackComponent.h"
#include "PlacedTower.h"
#include "PolyPalsController.h"
#include "PolyPalsPlayerState.h"
#include "Subsystems/TowerDataManager.h"
#include "Tower/TowerPropertyData.h"
#include "Tower/TowerMaterialData.h"

#include "EnemyPawn.h"

#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SphereComponent.h"
#include "Engine/StaticMeshSocket.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/OverlapResult.h"
#include "Engine/EngineTypes.h"
#include "Kismet/KismetSystemLibrary.h"

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
}

void UTowerAttackComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UTowerAttackComponent, CurrentLevel);
	DOREPLIFETIME(UTowerAttackComponent, TowerId);
	DOREPLIFETIME(UTowerAttackComponent, CurrentTarget);
	DOREPLIFETIME(UTowerAttackComponent, bMuzzleEffect);
	DOREPLIFETIME(UTowerAttackComponent, bAoeEffect);
	DOREPLIFETIME(UTowerAttackComponent, AoeLocation);
}

void UTowerAttackComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (GetWorld())
		GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void UTowerAttackComponent::ServerOnEnemyBeginOverlap(AActor* InEnemy)
{
	if (IsValid(InEnemy))
	{
		SpottedEnemy_Server.AddUnique(Cast<AEnemyPawn>(InEnemy));

		if (TowerState_Server == ETowerState::Idle)
			SetTowerState(ETowerState::SpotTarget);
	}
}

void UTowerAttackComponent::ServerOnEnemyEndOverlap(AActor* InEnemy)
{
	AEnemyPawn* EnemyPawn = Cast<AEnemyPawn>(InEnemy);
	SpottedEnemy_Server.Remove(EnemyPawn);
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
	UTowerDataManager* DataManager = GetWorld()->GetSubsystem<UTowerDataManager>();
	UTowerPropertyData* Data = DataManager->GetTowerPropertyData(TowerId);
	FTowerUpgradeValue* UpgradeData = Data->UpgradeData.Find(ELevelValue::Level1);
	Damage = UpgradeData->Damage;
	AttackDelay = UpgradeData->AttackDelay;
	TowerAbility = Data->TowerAbility;
	switch (TowerAbility)
	{
	case ETowerAbility::Slow:
		AbilityDuration = DataManager->GetAbilityDuration(TowerId, 1);
		AbilityIntensity = DataManager->GetAbilityIntensity(TowerId, 1);
		break;
	case ETowerAbility::Stun:
		AbilityDuration = DataManager->GetAbilityDuration(TowerId, 1);
		break;
	}
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
	case ETowerState::Delay:
		OnDelay();
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
	SpottedEnemy_Server.RemoveAll([](AActor* Candidate) {
		return !IsValid(Candidate);
		});

	if (SpottedEnemy_Server.IsEmpty())
		SetTowerState(ETowerState::LostTarget);
	else
		SetTowerState(ETowerState::Attack);
}

void UTowerAttackComponent::OnAttack()
{
	SpottedEnemy_Server.RemoveAll([](AActor* Candidate) {
		return !IsValid(Candidate);
		});

	if (SpottedEnemy_Server.IsEmpty())
	{
		SetTowerState(ETowerState::LostTarget);
		return;
	}

	if (IsValid(SpottedEnemy_Server[0]))
	{
		if (TowerAbility != ETowerAbility::None)
		{
			FCollisionShape Sphere = FCollisionShape::MakeSphere(250.f);
			TArray<FOverlapResult> OverlapResults;

			FCollisionQueryParams Params;
			Params.AddIgnoredActor(GetOwner());
			FCollisionResponseParams ResParams;
			ResParams.CollisionResponse = ECR_Overlap;

			//DrawDebugSphere(GetWorld(), SpottedEnemy_Server[0]->GetActorLocation(), 250.f, 8, FColor::Yellow, false, 1.5f);
			
			GetWorld()->OverlapMultiByChannel(OverlapResults,
				SpottedEnemy_Server[0]->GetActorLocation(), FQuat::Identity, ECollisionChannel::ECC_Pawn,
				Sphere, Params, ResParams);

			switch (TowerAbility)
			{
			case ETowerAbility::Slow:
				for (const auto& Iter : OverlapResults)
				{
					USphereComponent* Collision = Cast<USphereComponent>(Iter.GetComponent());
					if (Collision)
					{
						AEnemyPawn* Enemy = Cast<AEnemyPawn>(Collision->GetOwner());
						if (Enemy)
						{
							FString Name = Enemy->GetName();
							UE_LOG(LogTemp, Log, TEXT("Slow AOE overlapped with enemy: %s "), *Name);
							Enemy->ApplySlow(AbilityIntensity, AbilityDuration);
						}
					}
				}
				break;
			case ETowerAbility::Stun:
				for (const auto& Iter : OverlapResults)
				{
					USphereComponent* Collision = Cast<USphereComponent>(Iter.GetComponent());
					if (Collision)
					{
						AEnemyPawn* Enemy = Cast<AEnemyPawn>(Collision->GetOwner());
						if (Enemy)
						{
							FString Name = Enemy->GetName();
							UE_LOG(LogTemp, Log, TEXT("Stun AOE overlapped with enemy: %s "), *Name);
							Enemy->ApplyStun(AbilityDuration);
						}
					}
				}
				break;
			}
			Multicast_PlayAoeEffect(SpottedEnemy_Server[0]->GetActorLocation());
		}

		FVector BoxExtent = FVector(35.f, 35.f, 35.f);
		//DrawDebugBox(GetWorld(), SpottedEnemy_Server[0]->GetActorLocation(), BoxExtent, FColor::Red, false, 0.4f);
		CurrentTarget = SpottedEnemy_Server[0];
		SpottedEnemy_Server[0]->ReceiveDamage(Damage);
		UE_LOG(LogTemp, Log, TEXT("ApplyDamage: %f"), Damage);
		bMuzzleEffect = !bMuzzleEffect;
		SetTowerState(ETowerState::Delay);
	}
	else
		SetTowerState(ETowerState::LostTarget);
}

void UTowerAttackComponent::OnDelay()
{
	if (!GetWorld()) return;

	GetWorld()->GetTimerManager().SetTimer(DelayHandle, FTimerDelegate::CreateLambda([this]() {

		if (SpottedEnemy_Server.IsEmpty())
			SetTowerState(ETowerState::LostTarget);
		else
			SetTowerState(ETowerState::Attack);

		}), AttackDelay, false);
		
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
	//MuzzleEffectComponent->ActivateSystem();
	UNiagaraFunctionLibrary::SpawnSystemAttached(MuzzleEffect, GunMeshComponent, FName("MuzzleSocket"),
		FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true, true,
		ENCPoolMethod::AutoRelease);
}

void UTowerAttackComponent::OnRep_TowerId()
{
	if (GetWorld())
	{
		UTowerPropertyData* TowerData = GetWorld()->GetSubsystem<UTowerDataManager>()->GetTowerPropertyData(TowerId);
		FTowerUpgradeValue* UpgradeData = TowerData->UpgradeData.Find(ELevelValue::Level1);
		AttackDelay = UpgradeData->AttackDelay;
		MuzzleEffect = TowerData->MuzzleEffect;
		AoeEffect = TowerData->AoeEffect;
	}
}

void UTowerAttackComponent::OnRep_CurrentTarget()
{
	if (IsValid(CurrentTarget))
		SetGunMeshTimer();
	else
		ClearTowerTimer(GunMeshHandle);
}

void UTowerAttackComponent::Server_OnTowerLevelUp_Implementation()
{
	if (!GetOwner()->HasAuthority()) return;
	if (TowerId <= 0) return;
	if (!GetWorld()) return;

	CurrentLevel++;
	UTowerDataManager* DataManager = GetWorld()->GetSubsystem<UTowerDataManager>();
	UTowerPropertyData* Data = DataManager->GetTowerPropertyData(TowerId);
	
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

	switch (TowerAbility)
	{
	case ETowerAbility::Slow:
		AbilityDuration = DataManager->GetAbilityDuration(TowerId, CurrentLevel);
		AbilityIntensity = DataManager->GetAbilityIntensity(TowerId, CurrentLevel);
		break;
	case ETowerAbility::Stun:
		AbilityDuration = DataManager->GetAbilityDuration(TowerId, CurrentLevel);
		break;
	}
}

void UTowerAttackComponent::OnRep_CurrentLevel()
{
	if (!GetWorld()) return;
	if (TowerId <= 0) return;

	UTowerDataManager* DataManager = GetWorld()->GetSubsystem<UTowerDataManager>();
	UTowerPropertyData* PropertyData = DataManager->GetTowerPropertyData(TowerId);
	FTowerUpgradeValue* TowerUpgradeValue = PropertyData->UpgradeData.Find(static_cast<ELevelValue>(CurrentLevel-1));
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

void UTowerAttackComponent::OnRep_bAoeEffect()
{
	UE_LOG(LogTemp, Log, TEXT("bAoeEffect Callback"))
	if (bAoeEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), AoeEffect, AoeLocation, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true,
			ENCPoolMethod::AutoRelease);
	}
}

void UTowerAttackComponent::Multicast_PlayAoeEffect_Implementation(FVector_NetQuantize InLocation)
{
	UE_LOG(LogTemp, Log, TEXT("Multicast"));
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), AoeEffect, InLocation, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true,
		ENCPoolMethod::AutoRelease);
}

void UTowerAttackComponent::ClientOnClickedUpgrade()
{
	uint8 MaxLevel = static_cast<uint8>(ELevelValue::MaxLevel);
	if (CurrentLevel >= MaxLevel) return;
	
	ELevelValue NextLevel = static_cast<ELevelValue>(CurrentLevel);	// Already +1 by enum
	int32 Cost = GetWorld()->GetSubsystem<UTowerDataManager>()->GetTowerCost(TowerId, NextLevel);

	APolyPalsController* ProjectController = GetOwner()->GetOwner<APolyPalsController>();
	
	if (ProjectController)
	{
		APolyPalsPlayerState* ProjectPlayerState = ProjectController->GetPlayerState<APolyPalsPlayerState>();
		int32 CurrentGold = ProjectPlayerState->GetPlayerGold();

		if (CurrentGold >= Cost)
		{
			ProjectPlayerState->AddGold(-Cost);
			Server_OnTowerLevelUp();
			OwnerTower->TowerUpWidgetComponent->SetHiddenInGame(true);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("업그레이드 실패: 골드가 부족함"));
			OwnerTower->TowerUpWidgetComponent->SetHiddenInGame(true);
		}

	}
}