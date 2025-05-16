// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower/PreviewBuilding.h"
#include "Multiplayer/PolyPalsController.h"

// Sets default values
APreviewBuilding::APreviewBuilding()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));

	ConstructorHelpers::FObjectFinder<UStaticMesh> TowerMesh(TEXT("/Game/Meshs/Static/AssembledTower/SM_DefaultTower.SM_DefaultTower"));
	if (TowerMesh.Succeeded())
		MeshComponent->SetStaticMesh(TowerMesh.Object);

}

// Called when the game starts or when spawned
void APreviewBuilding::BeginPlay()
{
	Super::BeginPlay();

	//SetActorHiddenInGame(true);
	//SetActorTickEnabled(false);
}

// Called every frame
void APreviewBuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PolyPalsController)
	{
		UpdateLocationUnderCursor();
	}
}

void APreviewBuilding::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

}

void APreviewBuilding::ExteranlInitialize(APolyPalsController* const InController)
{
	PolyPalsController = InController;
}

void APreviewBuilding::ShowPreviewBuilding(bool bShow)
{
	SetActorHiddenInGame(!bShow);
	SetActorTickEnabled(bShow);
}

FVector APreviewBuilding::GetSnappedLocation(const FVector& WorldLocation)
{
	float GridSize = 100.f;
	FVector Snapped;
	Snapped.X = FMath::RoundToFloat(WorldLocation.X / GridSize) * GridSize;
	Snapped.Y = FMath::RoundToFloat(WorldLocation.Y / GridSize) * GridSize;
	Snapped.Z = WorldLocation.Z;

	return Snapped;
}

void APreviewBuilding::UpdateLocationUnderCursor()
{
	FHitResult HitResult;
	PolyPalsController->GetHitResultUnderCursor(ECollisionChannel::ECC_GameTraceChannel1, false, HitResult);
	
	FVector Snapped = GetSnappedLocation(HitResult.ImpactPoint);
	SetActorLocation(Snapped);
}

