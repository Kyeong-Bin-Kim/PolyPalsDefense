// Fill out your copyright notice in the Description page of Project Settings.


#include "Multiplayer/Components/PolyPalsController/GamePawnComponent.h"
#include "Multiplayer/PolyPalsGamePawn.h"
#include "Multiplayer/PolyPalsController.h"

#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UGamePawnComponent::UGamePawnComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	ConstructorHelpers::FClassFinder<APolyPalsGamePawn> GamePawnClass(TEXT("/Game/Multiplayer/BP_PolyPalsGamePawn.BP_PolyPalsGamePawn_C"));
	if (GamePawnClass.Succeeded())
		GamePawnClassDefault = GamePawnClass.Class;
}


// Called when the game starts
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

// Called every frame
void UGamePawnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	//Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UGamePawnComponent::ServerSpawnGamepawn()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	GamePawn = GetWorld()->SpawnActor<APolyPalsGamePawn>(GamePawnClassDefault, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

	GetOwner<APolyPalsController>()->Possess(GamePawn);
}



