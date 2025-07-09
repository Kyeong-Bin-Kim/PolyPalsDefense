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
    // 濡쒖뺄 ?뚮젅?댁뼱 而⑦듃濡ㅻ윭 媛?몄삤湲?
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

    if (!PC)
        return;

    // 而ㅼ꽌 ?꾨옒 ?덊듃 ?뚯뒪??
    FHitResult Hit;

    PC->GetHitResultUnderCursor(
        ECollisionChannel::ECC_GameTraceChannel3,
        /* bTraceComplex = */ true,
        Hit);

    // ???罹먯뒪??諛??뚯쑀沅?寃??
    APlacedTower* ClickedTower = Cast<APlacedTower>(Hit.GetActor());

    if (ClickedTower && ClickedTower->GetOwner() == PC)
    {
        // ?댁쟾 ?좉? ?④린湲?
        if (FocusedTower)
            FocusedTower->SetWidgetHidden(true);

        FocusedTower = ClickedTower;
        FocusedTower->SetWidgetHidden(false);

        // ?낃렇?덉씠???꾩젽 ?앹꽦/?좉?
        if (UpgradeWidgetInstance)
        {
            UpgradeWidgetInstance->RemoveFromParent();
            UpgradeWidgetInstance = nullptr;
        }
        if (TowerUpgradeWidgetClass)
        {
            UpgradeWidgetInstance = CreateWidget<UTowerUpgradeWidget>(PC, TowerUpgradeWidgetClass);
            if (UpgradeWidgetInstance)
            {
                UpgradeWidgetInstance->SetTargetTower(FocusedTower);
                UpgradeWidgetInstance->AddToViewport();
            }
        }
    }
    else
    {
        // ???諛붽묑???대┃?섎㈃ 湲곗〈 ?좉? ?④린湲?
        if (FocusedTower)
        {
            FocusedTower->SetWidgetHidden(true);
            FocusedTower = nullptr;
        }
        if (UpgradeWidgetInstance)
        {
            UpgradeWidgetInstance->RemoveFromParent();
            UpgradeWidgetInstance = nullptr;
        }
    }
}

