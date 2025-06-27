#include "StageSelectUIWidget.h"
#include "PolyPalsController.h"
#include "PolyPalsGameInstance.h"
#include "PolyPalsState.h"
#include "PolyPalsPlayerState.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

void UStageSelectUIWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (DirtStage){DirtStage->OnClicked.AddDynamic(this, &UStageSelectUIWidget::OnDirtClicked);}
    if (SnowStage){SnowStage->OnClicked.AddDynamic(this, &UStageSelectUIWidget::OnSnowClicked);}
    if (ExitGame){ExitGame->OnClicked.AddDynamic(this, &UStageSelectUIWidget::OnExitGameClicked);}
}

void UStageSelectUIWidget::OnDirtClicked()
{
    HandleStageSelected(TEXT("Dirt"));
}

void UStageSelectUIWidget::OnSnowClicked()
{
    HandleStageSelected(TEXT("Snow"));
}


void UStageSelectUIWidget::OnExitGameClicked()
{
    if (APolyPalsController* PC = Cast<APolyPalsController>(GetOwningPlayer()))
    {
        PC->ShowMainUI();
    }
}

void UStageSelectUIWidget::HandleStageSelected(FName StageName)
{
    LastSelectedStage = StageName;

    if (APolyPalsController* PC = Cast<APolyPalsController>(GetOwningPlayer()))
    {
        PC->Server_SetSelectedStage(StageName);

        if (PC->HasAuthority())
        {
            if (UPolyPalsGameInstance* GI = GetWorld()->GetGameInstance<UPolyPalsGameInstance>())
            {
                GI->CreateSteamSession();
            }
        }

        FString PlayerName = TEXT("Unknown");

        if (APolyPalsPlayerState* PS = PC->GetPlayerState<APolyPalsPlayerState>())
        {
            PlayerName = PS->GetPlayerName();
        }

        if (APolyPalsState* GS = GetWorld()->GetGameState<APolyPalsState>())
        {
            GS->SetLobbyName(PlayerName);
        }

        PC->ConfigureLobbyUI(StageName, PlayerName);
    }
}
