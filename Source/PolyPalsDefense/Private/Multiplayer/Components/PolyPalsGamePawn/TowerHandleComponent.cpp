// Fill out your copyright notice in the Description page of Project Settings.


#include "Multiplayer/Components/PolyPalsGamePawn/TowerHandleComponent.h"
#include "Multiplayer/PolyPalsGamePawn.h"
#include "Multiplayer/PolyPalsController.h"
#include "Tower/PlacedTower.h"

#include "Net/UnrealNetwork.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/OverlapResult.h"

UTowerHandleComponent::UTowerHandleComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	//bWantsInitializeComponent = true;
}

void UTowerHandleComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void UTowerHandleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	//Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UTowerHandleComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(UTowerHandleComponent, PolyPalsController, COND_OwnerOnly);
}

void UTowerHandleComponent::ClientOnInputClick()
{
	//UE_LOG(LogTemp, Log, TEXT("UTowerHandleComponent detected click"));

	if (BuildState == EBuildState::None)
	{
		FHitResult HitResult;
		PolyPalsController->GetHitResultUnderCursor(ECollisionChannel::ECC_GameTraceChannel3, true, HitResult);
		AActor* ClickedActor = HitResult.GetActor();
		APlacedTower* ClickedTower = Cast<APlacedTower>(ClickedActor);
		if (ClickedTower && ClickedTower->GetOwner())
		{
			if (FocusedTower)
				FocusedTower->SetWidgetHidden(true);
			FocusedTower = ClickedTower;
			FocusedTower->SetWidgetHidden(false);
		}
		else
		{
			if (FocusedTower)
				FocusedTower->SetWidgetHidden(true);
		}
	}
}

