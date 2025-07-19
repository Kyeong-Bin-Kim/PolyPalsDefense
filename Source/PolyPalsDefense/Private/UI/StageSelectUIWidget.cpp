#include "StageSelectUIWidget.h"
#include "PolyPalsController.h"
#include "PolyPalsGameInstance.h"
#include "PolyPalsState.h"
#include "PolyPalsPlayerState.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "GenericPlatform/GenericPlatformHttp.h"

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

    // 로비 이름 생성
    FString LobbyName = TEXT("Host");

    if (APlayerState* PS = GetOwningPlayer()->PlayerState)
    {
        LobbyName = PS->GetPlayerName();
    }

    // URL‑encode (공백이나 특수문자 처리를 위해)
    FString EncodedLobby = FGenericPlatformHttp::UrlEncode(LobbyName);
    FString EncodedStage = FGenericPlatformHttp::UrlEncode(StageName.ToString());

    // 옵션 붙이기
    FString URL = FString::Printf(
        TEXT("127.0.0.1:7777?SelectedStage=%s?LobbyName=%s"),
        *EncodedStage,
        *EncodedLobby
    );

    UE_LOG(LogTemp, Warning, TEXT("[HOST] 서버 연결 URL: %s"), *URL);

    // 4) ClientTravel 호출
    if (APolyPalsController* PC = Cast<APolyPalsController>(GetOwningPlayer()))
    {
        PC->ClientTravel(*URL, ETravelType::TRAVEL_Absolute);
    }
}
