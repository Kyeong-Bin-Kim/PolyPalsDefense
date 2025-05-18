// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower/PreviewBuilding.h"
#include "Multiplayer/PolyPalsController.h"
#include "Core/Subsystems/TowerDataManager.h"
#include "DataAsset/Tower/TowerPropertyData.h"
#include "DataAsset/Tower/TowerMaterialData.h"

// Sets default values
APreviewBuilding::APreviewBuilding()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));

	ConstructorHelpers::FObjectFinder<UStaticMesh> TowerMesh(TEXT("/Game/Meshs/Static/AssembledTower/SM_NullTower.SM_NullTower"));
	if (TowerMesh.Succeeded())
		MeshComponent->SetStaticMesh(TowerMesh.Object);

}

// Called when the game starts or when spawned
void APreviewBuilding::BeginPlay()
{
	Super::BeginPlay();

	TowerDataManager = GetWorld()->GetSubsystem<UTowerDataManager>();
	ShowPreviewBuilding(false);
	PolyPalsController = Cast<APolyPalsController>(GetWorld()->GetFirstPlayerController());
	SetOwner(PolyPalsController);
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

void APreviewBuilding::ShowPreviewBuilding(bool bShow, uint8 InTowerId)
{
	SetActorHiddenInGame(!bShow);
	SetActorTickEnabled(bShow);

	if (!bShow)
		SetActorLocation(OffsetLocation);

	if (!TowerDataManager) return;

	if (bShow)
	{
		UTowerPropertyData* PropertyData = TowerDataManager->GetTowerPropertyData(InTowerId);
		MeshComponent->SetStaticMesh(PropertyData->TowerMesh);
		if (!BuildableMat || !UnbuildableMat)
		{
			UTowerMaterialData* MaterialData = TowerDataManager->GetTowerMaterialData();
			BuildableMat = MaterialData->Buildable;
			UnbuildableMat = MaterialData->Unbuildable;
		}
		
		uint8 MaterialCount = MeshComponent->GetNumMaterials();
		for (uint8 Iter = 0; Iter < MaterialCount; Iter++)
		{
			MeshComponent->SetMaterial(Iter, BuildableMat);
		}
	}
}

void APreviewBuilding::ChangeMeshMaterial(bool bIsBuildable)
{
	if (!BuildableMat || !UnbuildableMat)
	{
		UTowerMaterialData* MaterialData = TowerDataManager->GetTowerMaterialData();
		BuildableMat = MaterialData->Buildable;
		UnbuildableMat = MaterialData->Unbuildable;
	}

	uint8 MaterialCount = MeshComponent->GetNumMaterials();

	if (bIsBuildable)
	{
		for (uint8 Iter = 0; Iter < MaterialCount; Iter++)
		{
			MeshComponent->SetMaterial(Iter, BuildableMat);
		}
	}
	else
	{
		for (uint8 Iter = 0; Iter < MaterialCount; Iter++)
		{
			MeshComponent->SetMaterial(Iter, UnbuildableMat);
		}
	}
}

FVector APreviewBuilding::GetSnappedLocation(const FVector& WorldLocation)
{
	float GridSize = 110.f;
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

