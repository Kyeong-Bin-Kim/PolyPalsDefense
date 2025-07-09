#include "Multiplayer/PolyPalsGamePawn.h"
#include "Multiplayer/PolyPalsController.h"
#include "Multiplayer/Components/PolyPalsGamePawn/BuildTowerComponent.h"
#include "Multiplayer/Components/PolyPalsGamePawn/TowerHandleComponent.h"
#include "DataAsset/Tower/TowerEnums.h"

#include "Net/UnrealNetwork.h"
#include "Components/SceneComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

APolyPalsGamePawn::APolyPalsGamePawn()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	SetRootComponent(RootSceneComponent);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArm->SetupAttachment(RootSceneComponent);

	PolyPalsPlayCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PolyPalsPlayCameara"));
	PolyPalsPlayCamera->SetupAttachment(SpringArm);

	BuildTowerComponent = CreateDefaultSubobject<UBuildTowerComponent>(TEXT("BuildTowerComponent"));
	BuildTowerComponent->SetIsReplicated(true);

	TowerHandleComponent = CreateDefaultSubobject<UTowerHandleComponent>(TEXT("TowerHandleComponent"));
	TowerHandleComponent->SetIsReplicated(true);
}

void APolyPalsGamePawn::BeginPlay()
{
	Super::BeginPlay();
	
	SetActorLocation(SpectateLocation);
}