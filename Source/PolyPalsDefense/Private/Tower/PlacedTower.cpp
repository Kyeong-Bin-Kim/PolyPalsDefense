// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower/PlacedTower.h"
#include "Components/SceneComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
APlacedTower::APlacedTower()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	SetRootComponent(RootSceneComponent);

	TowerMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TowerMeshComponent"));
	TowerMeshComponent->SetupAttachment(RootSceneComponent);

	GunMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GunMeshComponent"));
	GunMeshComponent->SetupAttachment(TowerMeshComponent);

	

}

// Called when the game starts or when spawned
void APlacedTower::BeginPlay()
{
	Super::BeginPlay();
	
}

void APlacedTower::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	
}

// Called every frame
void APlacedTower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

