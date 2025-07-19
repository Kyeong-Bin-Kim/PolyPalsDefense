#include "PolyPalsPlayerState.h"
#include "PolyPalsController.h"
#include "PolyPalsHUD.h"
#include "LobbyUIWidget.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

void APolyPalsPlayerState::BeginPlay()
{
    Super::BeginPlay();

    if (IsOwnedBy(UGameplayStatics::GetPlayerController(this, 0)))
    {
        OnRep_PlayerGold();
    }
}

void APolyPalsPlayerState::SetReadyState(bool bReady)
{
    if (HasAuthority())
    {
        bIsReady = bReady;

        OnRep_IsReady();
    }
}

void APolyPalsPlayerState::SetSlotIndex(int32 Index)
{
    if (HasAuthority())
    {
        SlotIndex = Index;
    }
}

void APolyPalsPlayerState::AddGold(int32 Amount)
{
    PlayerGold += Amount;


    OnRep_PlayerGold();
}

void APolyPalsPlayerState::SetInitialGold(int32 Amount)
{
    if (HasAuthority())
    {
        PlayerGold = Amount;
        OnRep_PlayerGold();
    }
}

void APolyPalsPlayerState::OnRep_SlotIndex()
{
    if (APlayerController* PC = Cast<APlayerController>(GetOwner()))
    {
        if (APolyPalsController* PPC = Cast<APolyPalsController>(PC))
        {
            PPC->RefreshLobbyUI();
        }
    }
}

void APolyPalsPlayerState::OnRep_IsReady()
{
    UWorld* World = GetWorld();
    if (!World) return;

    for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
    {
        if (APolyPalsController* PPC = Cast<APolyPalsController>(*It))
        {
            if (PPC->IsLocalController())
            {
                PPC->UpdateReadyUI(this, bIsReady);
            }
        }
    }
}

void APolyPalsPlayerState::OnRep_PlayerGold()
{
    if (APlayerController* PC = Cast<APlayerController>(GetOwner()))
    {
        if (APolyPalsHUD* HUD = Cast<APolyPalsHUD>(PC->GetHUD()))
        {
            HUD->UpdateGoldOnUI(PlayerGold);
        }
    }
}

void APolyPalsPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(APolyPalsPlayerState, bIsReady);
    DOREPLIFETIME(APolyPalsPlayerState, PlayerGold);
    DOREPLIFETIME(APolyPalsPlayerState, SlotIndex);
}

bool APolyPalsPlayerState::SpendGold(int32 Amount)
{
    if (!HasAuthority() || PlayerGold < Amount)
        return false;

    PlayerGold -= Amount;
    OnRep_PlayerGold();
    return true;
}