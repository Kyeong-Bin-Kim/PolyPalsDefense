// Fill out your copyright notice in the Description page of Project Settings.


#include "Multiplayer/PolyPalsGamePawn.h"
#include "Net/UnrealNetwork.h"
#include "Components/SceneComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
APolyPalsGamePawn::APolyPalsGamePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	SetRootComponent(RootSceneComponent);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArm->SetupAttachment(RootSceneComponent);

	PolyPalsPlayCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PolyPalsPlayCameara"));
	PolyPalsPlayCamera->SetupAttachment(SpringArm);
}

// Called when the game starts or when spawned
void APolyPalsGamePawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APolyPalsGamePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APolyPalsGamePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

