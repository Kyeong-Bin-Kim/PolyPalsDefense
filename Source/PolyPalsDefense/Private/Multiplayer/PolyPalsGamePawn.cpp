// Fill out your copyright notice in the Description page of Project Settings.


#include "Multiplayer/PolyPalsGamePawn.h"
#include "Multiplayer/PolyPalsController.h"
#include "Multiplayer/Components/PolyPalsGamePawn/BuildTowerComponent.h"
#include "Multiplayer/Components/PolyPalsController/PolyPalsInputComponent.h"
#include "DataAsset/Tower/TowerEnums.h"

#include "Net/UnrealNetwork.h"
#include "Components/SceneComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
APolyPalsGamePawn::APolyPalsGamePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	SetRootComponent(RootSceneComponent);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArm->SetupAttachment(RootSceneComponent);

	PolyPalsPlayCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PolyPalsPlayCameara"));
	PolyPalsPlayCamera->SetupAttachment(SpringArm);

	BuildTowerComponent = CreateDefaultSubobject <UBuildTowerComponent>(TEXT("BuildTowerComponent"));
	BuildTowerComponent->SetIsReplicated(true);
}

// Called when the game starts or when spawned
void APolyPalsGamePawn::BeginPlay()
{
	Super::BeginPlay();
	
	SetActorLocation(SpectateLocation);
}

void APolyPalsGamePawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APolyPalsGamePawn, bIsPossessed);
	DOREPLIFETIME_CONDITION(APolyPalsGamePawn, PolyPalsController, COND_OwnerOnly);
}

// Called every frame
void APolyPalsGamePawn::Tick(float DeltaTime)
{
	//Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APolyPalsGamePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	

}

void APolyPalsGamePawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	bIsPossessed = true;

	PolyPalsController = Cast<APolyPalsController>(NewController);
}

void APolyPalsGamePawn::UnPossessed()
{
	Super::UnPossessed();

	bIsPossessed = false;
	UnbindInputDelegate();
	PolyPalsController = nullptr;
	//BuildTowerComponent->ServerSetPlayerColor(EPlayerColor::None);
}

void APolyPalsGamePawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UnbindInputDelegate();
}

void APolyPalsGamePawn::UnbindInputDelegate()
{
	if (PolyPalsController)
	{
		UPolyPalsInputComponent* polypalsInputcomp = PolyPalsController->GetPolyPalsInputComponent();
		if (polypalsInputcomp)
		{
			polypalsInputcomp->OnInputTest.Unbind();
			polypalsInputcomp->OnInputClick.Unbind();
			polypalsInputcomp->OnInputRightClick.Unbind();
			polypalsInputcomp->OnInputTower1.Unbind();
			polypalsInputcomp->OnInputTower2.Unbind();
			polypalsInputcomp->OnInputTower3.Unbind();
		}
	}
}

void APolyPalsGamePawn::OnRep_PolyPalsController()
{
	if (PolyPalsController)
	{
		UPolyPalsInputComponent* polypalsInputcomp = PolyPalsController->GetPolyPalsInputComponent();
		if (polypalsInputcomp)
		{
			polypalsInputcomp->OnInputTest.BindUObject(BuildTowerComponent, &UBuildTowerComponent::ClientOnInputTest);
			polypalsInputcomp->OnInputClick.BindUObject(BuildTowerComponent, &UBuildTowerComponent::ClientOnInputClick);
			polypalsInputcomp->OnInputRightClick.BindUObject(BuildTowerComponent, &UBuildTowerComponent::ClientOnInputRightClick);
			polypalsInputcomp->OnInputTower1.BindUObject(BuildTowerComponent, &UBuildTowerComponent::ClientOnInputTower1);
			polypalsInputcomp->OnInputTower2.BindUObject(BuildTowerComponent, &UBuildTowerComponent::ClientOnInputTower2);
			polypalsInputcomp->OnInputTower3.BindUObject(BuildTowerComponent, &UBuildTowerComponent::ClientOnInputTower3);
		}

		BuildTowerComponent->ClientSpawnPreviewBuilding();
	}
}
