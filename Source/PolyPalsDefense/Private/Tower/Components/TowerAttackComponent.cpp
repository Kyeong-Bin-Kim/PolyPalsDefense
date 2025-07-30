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
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
}

void UTowerAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	ENetMode Mode = GetWorld()->GetNetMode();

	// ?쒕쾭?먯꽌????爰쇰몢湲?
	if (Mode == NM_DedicatedServer)
	{
		PrimaryComponentTick.SetTickFunctionEnable(false);
	}
	else
	{
		PrimaryComponentTick.SetTickFunctionEnable(true);
	}

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
	DOREPLIFETIME(UTowerAttackComponent, MuzzleEffect);
	DOREPLIFETIME(UTowerAttackComponent, AoeEffect);
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
	if (!GunMeshComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("GunMeshComponent is NULL!"));
		return;
	}

	if (!IsValid(CurrentTarget))
	{
		return;
	}

	FVector GunLoc = GunMeshComponent->GetComponentLocation();
	FVector TargetLoc = CurrentTarget->GetActorLocation();
	FRotator WantRot = UKismetMathLibrary::FindLookAtRotation(GunLoc, TargetLoc);
	FRotator Desired = FRotator(0.f, WantRot.Yaw, 0.f);

	FRotator Current = GunMeshComponent->GetRelativeRotation();
	FRotator NewRot = FMath::RInterpTo(Current, Desired, GetWorld()->GetDeltaSeconds(), 10.f);

	GunMeshComponent->SetRelativeRotation(NewRot);
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
	MuzzleEffect = Data->MuzzleEffect;
	AoeEffect = Data->AoeEffect;
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

void UTowerAttackComponent::ServerSetCurrentTarget_Implementation(AEnemyPawn* NewTarget)
{
	CurrentTarget = NewTarget;
	GetOwner()->ForceNetUpdate();
	HandleNewTarget();
}

void UTowerAttackComponent::OnAttack()
{
	// ?좏슚?섏? ?딆? ?ъ씤???쒓굅
	SpottedEnemy_Server.RemoveAll([](AActor* Candidate) {
		return !IsValid(Candidate);
		});

	// ??곸씠 ?놁쑝硫?LostTarget
	if (SpottedEnemy_Server.IsEmpty())
	{
		SetTowerState(ETowerState::LostTarget);
		return;
	}

	// ?ㅽ뵆?쇱씤 吏꾪뻾 嫄곕━媛 媛??????李얘린
	AEnemyPawn* Best = nullptr;
	float MaxDist = -1.f;
	for (AActor* Candidate : SpottedEnemy_Server)
	{
		AEnemyPawn* EnemyCandidate = Cast<AEnemyPawn>(Candidate);
		if (IsValid(EnemyCandidate))
		{
			float D = EnemyCandidate->GetDistanceAlongPath();
			if (D > MaxDist)
			{
				MaxDist = D;
				Best = EnemyCandidate;
			}
		}
	}

	// 紐?李얠븯?쇰㈃ LostTarget
	if (!IsValid(Best))
	{
		SetTowerState(ETowerState::LostTarget);
		return;
	}

	// ?щ줈???ㅽ꽩 AOE
	if (TowerAbility != ETowerAbility::None)
	{
		FCollisionShape Sphere = FCollisionShape::MakeSphere(250.f);
		TArray<FOverlapResult> OverlapResults;

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(GetOwner());
		FCollisionResponseParams ResParams;
		ResParams.CollisionResponse = ECR_Overlap;

		GetWorld()->OverlapMultiByChannel(
			OverlapResults,
			Best->GetActorLocation(),
			FQuat::Identity,
			ECollisionChannel::ECC_Pawn,
			Sphere,
			Params,
			ResParams
		);

		switch (TowerAbility)
		{
		case ETowerAbility::Slow:

			for (const auto& Iter : OverlapResults)
			{
				if (USphereComponent* Collision = Cast<USphereComponent>(Iter.GetComponent()))
					if (AEnemyPawn* Enemy = Cast<AEnemyPawn>(Collision->GetOwner()))
					{
						Enemy->ApplySlow(AbilityIntensity, AbilityDuration);
					}
			}

			break;

		case ETowerAbility::Stun:

			for (const auto& Iter : OverlapResults)
			{
				if (USphereComponent* Collision = Cast<USphereComponent>(Iter.GetComponent()))
					if (AEnemyPawn* Enemy = Cast<AEnemyPawn>(Collision->GetOwner()))
					{
						Enemy->ApplyStun(AbilityDuration);
					}
			}

			break;
		}

		Multicast_PlayAoeEffect(Best->GetActorLocation());
	}

	Multicast_PlayMuzzleEffect();

	ServerSetCurrentTarget(Best);
	Best->ReceiveDamage(Damage);
	bMuzzleEffect = !bMuzzleEffect;

	SetTowerState(ETowerState::Delay);
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

void UTowerAttackComponent::HandleNewTarget()
{
	if (IsValid(CurrentTarget))
		SetGunMeshTimer();
	else
		ClearTowerTimer(GunMeshHandle);
}

void UTowerAttackComponent::OnRep_CurrentTarget()
{
	HandleNewTarget();
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
	FTowerUpgradeValue* TowerUpgradeValue = PropertyData->UpgradeData.Find(static_cast<ELevelValue>(CurrentLevel - 1));
	AttackDelay = TowerUpgradeValue->AttackDelay;

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

void UTowerAttackComponent::OnRep_MuzzleEffect()
{
	UNiagaraFunctionLibrary::SpawnSystemAttached(MuzzleEffect, GunMeshComponent, FName("MuzzleSocket"),
		FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true, true,
		ENCPoolMethod::AutoRelease);
}

void UTowerAttackComponent::OnRep_bAoeEffect()
{
	if (bAoeEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), AoeEffect, AoeLocation, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true,
			ENCPoolMethod::AutoRelease);
	}
}

void UTowerAttackComponent::Multicast_PlayMuzzleEffect_Implementation()
{
	if (!MuzzleEffect)
	{
		UE_LOG(LogTemp, Error, TEXT("[Muzzle RPC] MuzzleEffect is null"));
		return;
	}

	UNiagaraFunctionLibrary::SpawnSystemAttached(MuzzleEffect, GunMeshComponent, FName("MuzzleSocket"),
		FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true, true,
		ENCPoolMethod::AutoRelease);
}

void UTowerAttackComponent::Multicast_PlayAoeEffect_Implementation(FVector_NetQuantize InLocation)
{
	if (!AoeEffect)
	{
		UE_LOG(LogTemp, Error, TEXT("[AOE RPC] AoeEffect is null"));
		return;
	}

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), AoeEffect, InLocation, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::AutoRelease);
}

void UTowerAttackComponent::ClientOnClickedUpgrade()
{
	uint8 MaxLevel = static_cast<uint8>(ELevelValue::MaxLevel);

	if (CurrentLevel >= MaxLevel) return;

	ELevelValue NextLevel = static_cast<ELevelValue>(CurrentLevel);
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
			OwnerTower->TowerUpWidgetComponent->SetHiddenInGame(true);
		}

	}
}