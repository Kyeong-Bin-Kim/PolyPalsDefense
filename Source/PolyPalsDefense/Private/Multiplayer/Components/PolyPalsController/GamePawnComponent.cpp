// Fill out your copyright notice in the Description page of Project Settings.


#include "Multiplayer/Components/PolyPalsController/GamePawnComponent.h"
#include "Multiplayer/PolyPalsGamePawn.h"
#include "Multiplayer/PolyPalsController.h"

#include "Net/UnrealNetwork.h"

UGamePawnComponent::UGamePawnComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	ConstructorHelpers::FClassFinder<APolyPalsGamePawn> GamePawnClass(TEXT("/Game/Multiplayer/BP_PolyPalsGamePawn.BP_PolyPalsGamePawn_C"));
	if (GamePawnClass.Succeeded())
		GamePawnClassDefault = GamePawnClass.Class;
}

void UGamePawnComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->HasAuthority())
		ServerSpawnGamepawn();
	
}


void UGamePawnComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UGamePawnComponent, GamePawn, COND_OwnerOnly);
}

void UGamePawnComponent::ServerSpawnGamepawn()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FVector SpawnLocation = GamePawnClassDefault->GetDefaultObject<APolyPalsGamePawn>()->GetSpectateLocation();
	GamePawn = GetWorld()->SpawnActor<APolyPalsGamePawn>(GamePawnClassDefault, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

	GetOwner<APolyPalsController>()->Possess(GamePawn);
}