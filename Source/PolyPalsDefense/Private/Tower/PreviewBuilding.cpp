// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower/PreviewBuilding.h"
#include "Multiplayer/PolyPalsController.h"
#include "Core/Subsystems/TowerDataManager.h"
#include "DataAsset/Tower/TowerPropertyData.h"
#include "DataAsset/Tower/TowerMaterialData.h"

#include "Components/SceneComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/OverlapResult.h"

// Sets default values
APreviewBuilding::APreviewBuilding()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	SetRootComponent(RootScene);
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootScene);
	MeshComponent->SetRelativeScale3D(FVector(0.85f, 0.85f, 0.85f));
	

	ConstructorHelpers::FObjectFinder<UStaticMesh> TowerMesh(TEXT("/Game/Meshs/Static/AssembledTower/SM_NullTower.SM_NullTower"));
	if (TowerMesh.Succeeded())
		MeshComponent->SetStaticMesh(TowerMesh.Object);

	OffsetLocation = FVector(0.f, 0.f, 5000.f);
}

// Called when the game starts or when spawned
void APreviewBuilding::BeginPlay()
{
	Super::BeginPlay();

	TowerDataManager = GetWorld()->GetSubsystem<UTowerDataManager>();
	ShowPreviewBuilding(false);
	PolyPalsController = Cast<APolyPalsController>(GetWorld()->GetFirstPlayerController());
	SetOwner(PolyPalsController);

	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

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
	{
		SetActorLocation(OffsetLocation);
		LastSnappedLocation = OffsetLocation;
	}

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

void APreviewBuilding::ChangeMeshMaterial(bool bIsCanBuild)
{
	if (!BuildableMat || !UnbuildableMat)
	{
		UTowerMaterialData* MaterialData = TowerDataManager->GetTowerMaterialData();
		BuildableMat = MaterialData->Buildable;
		UnbuildableMat = MaterialData->Unbuildable;
	}

	if (!BuildableMat || !UnbuildableMat) return;

	uint8 MaterialCount = MeshComponent->GetNumMaterials();

	if (bIsCanBuild)
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
	float GridSize = 100.f;

	FVector Snapped;
	Snapped.X = FMath::FloorToFloat(WorldLocation.X / GridSize) * GridSize + GridSize * 0.5f;
	Snapped.Y = FMath::FloorToFloat(WorldLocation.Y / GridSize) * GridSize + GridSize * 0.5f;
	Snapped.Z = WorldLocation.Z;

	return Snapped;
}

void APreviewBuilding::UpdateLocationUnderCursor()
{
	FHitResult HitResult;
	PolyPalsController->GetHitResultUnderCursor(ECollisionChannel::ECC_GameTraceChannel1, false, HitResult);
	
	FVector NewSnappedLocation = GetSnappedLocation(HitResult.ImpactPoint);

	if (!NewSnappedLocation.Equals(LastSnappedLocation))
	{
		SetActorLocation(NewSnappedLocation, true);
		LastSnappedLocation = NewSnappedLocation;

		FVector BoxExtent = FVector(35.f, 35.f, 100.f);
		FCollisionShape Box = FCollisionShape::MakeBox(BoxExtent);
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		FCollisionResponseParams ResParams;
		ResParams.CollisionResponse = ECR_Block;

		TArray<FOverlapResult> OverlapResults;

		bool bOverlaps = GetWorld()->OverlapMultiByChannel(OverlapResults, NewSnappedLocation, FQuat::Identity, ECollisionChannel::ECC_GameTraceChannel2, Box, Params, ResParams);
		bool bFlag = false;

		if (!bOverlaps)
			bFlag = true;

		if (bIsBuildable != bFlag)
		{
			bIsBuildable = bFlag;
			ChangeMeshMaterial(bFlag);
		}
	}
}

void APreviewBuilding::OnPlacedTowerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag(FName("Tower")))
	{
		bIsBuildable = false;
		ChangeMeshMaterial(false);
	}
}

