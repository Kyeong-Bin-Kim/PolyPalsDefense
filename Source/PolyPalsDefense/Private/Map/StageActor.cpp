#include "StageActor.h"
#include "Components/SplineComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Map/WaveSpawner.h"

AStageActor::AStageActor()
{
    PrimaryActorTick.bCanEverTick = false;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    PathSpline = CreateDefaultSubobject<USplineComponent>(TEXT("PathSpline"));
    PathSpline->SetupAttachment(RootComponent);

    TowerBuildZoneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("TowerBuildZoneRoot"));
    TowerBuildZoneRoot->SetupAttachment(RootComponent);

    PathBlockZoneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("PathBlockZoneRoot"));
    PathBlockZoneRoot->SetupAttachment(RootComponent);
}

void AStageActor::BeginPlay()
{
    Super::BeginPlay();

    TArray<UStaticMeshComponent*> Meshes;
    GetComponents<UStaticMeshComponent>(Meshes);

    for (UStaticMeshComponent* Mesh : Meshes)
    {
        if (Mesh->GetAttachParent() == TowerBuildZoneRoot)
        {
            Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
            Mesh->SetCollisionObjectType(ECC_WorldStatic);
            Mesh->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
            Mesh->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Ignore);
        }
        else if (Mesh->GetAttachParent() == PathBlockZoneRoot)
        {
            Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
            Mesh->SetCollisionObjectType(ECC_WorldStatic);
            Mesh->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
            Mesh->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Block);
        }
    }
}

void AStageActor::RegisterSpawner(AWaveSpawner* Spawner)
{
    if (Spawner && !WaveSpawners.Contains(Spawner))
    {
        WaveSpawners.Add(Spawner);
    }
}
