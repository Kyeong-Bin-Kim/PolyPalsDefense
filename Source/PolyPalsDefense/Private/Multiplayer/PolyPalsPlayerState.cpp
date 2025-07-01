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
        // 클라이언트 본인일 경우
        OnRep_PlayerGold(); // 수동 호출로 HUD 갱신
    }
}

void APolyPalsPlayerState::SetReadyState(bool bReady)
{
    if (HasAuthority())
    {
        bIsReady = bReady;
        ForceNetUpdate();
        OnRep_IsReady();
    }
}

void APolyPalsPlayerState::SetSlotIndex(int32 Index)
{
    if (HasAuthority())
    {
        SlotIndex = Index;
        UE_LOG(LogTemp, Log, TEXT("SlotIndex assigned: %d"), Index);
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
    UE_LOG(LogTemp, Log, TEXT("[PlayerState] SlotIndex 상태 변경됨: %d"), SlotIndex);

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
    UE_LOG(LogTemp, Log, TEXT("[PlayerState] Ready 상태 변경됨: %s"), bIsReady ? TEXT("O") : TEXT("X"));

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
    UE_LOG(LogTemp, Log, TEXT("[PlayerState] OnRep_PlayerGold: %d"), PlayerGold);

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

    DOREPLIFETIME(APolyPalsPlayerState, bIsReady); // Ready 상태 복제
    DOREPLIFETIME(APolyPalsPlayerState, PlayerGold); // 개인 골드 복제
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