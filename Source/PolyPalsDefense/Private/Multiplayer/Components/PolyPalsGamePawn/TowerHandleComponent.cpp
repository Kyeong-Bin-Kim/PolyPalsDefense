#include "Multiplayer/Components/PolyPalsGamePawn/TowerHandleComponent.h"
#include "Tower/PlacedTower.h"
#include "TowerUpgradeWidget.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/OverlapResult.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

UTowerHandleComponent::UTowerHandleComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTowerHandleComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTowerHandleComponent::HandleLeftClick()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

    if (!PC)
        return;

    FHitResult Hit;

    PC->GetHitResultUnderCursor(
        ECollisionChannel::ECC_GameTraceChannel3,
        /* bTraceComplex = */ true,
        Hit);

    APlacedTower* ClickedTower = Cast<APlacedTower>(Hit.GetActor());

    if (ClickedTower && ClickedTower->GetOwner() == PC)
    {
        if (FocusedTower)
        {
            FocusedTower->SetWidgetHidden(true);
        }
            
        FocusedTower = ClickedTower;
        FocusedTower->SetWidgetHidden(false);
    }
    else
    {
        if (FocusedTower)
        {
            FocusedTower->SetWidgetHidden(true);
            FocusedTower = nullptr;
        }
    }
}

