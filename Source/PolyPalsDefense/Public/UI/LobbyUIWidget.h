#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerSlotWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"
#include "LobbyUIWidget.generated.h"

class APolyPalsPlayerState;

UCLASS()
class POLYPALSDEFENSE_API ULobbyUIWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    // ?몃??먯꽌 ?좏깮???ㅽ뀒?댁? ?ㅼ젙
    UFUNCTION(BlueprintCallable)
    void SetSelectedStage(FName InStageName);

    UFUNCTION(BlueprintCallable)
    void SetRoomTitle(const FString& InRoomTitle);

    // ?몃??먯꽌 濡쒕퉬 ?곹깭 ?낅뜲?댄듃 (?뚮젅?댁뼱 ?? 以鍮??? ?ㅽ뀒?댁?, 諛⑹옣 ?щ?)
    UFUNCTION(BlueprintCallable)
    void UpdateLobbyInfo(int32 ConnectedPlayers, int32 ReadyPlayers, FName CurrentStage, bool bIsHost);

    void UpdatePlayerSlotReadyState(APolyPalsPlayerState* ChangedState, bool bNewReady);

    // PlayerArray ?뺣낫瑜??댁슜???щ’ UI瑜?媛깆떊
    void RefreshPlayerSlots(const TArray<APlayerState*>& PlayerStates);

protected:
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* RoomTitleText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* StageText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TotalReadyText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* MaxPlayerCountText;

    UPROPERTY(meta = (BindWidget))
    class UVerticalBox* PlayerSlotBox;

    UPROPERTY(meta = (BindWidget))
    class UButton* ExitGame;

    UFUNCTION()
    void OnExitGameClicked();

    UFUNCTION()
    void HandleSlotReadyClicked(UPlayerSlotWidget* ClickedSlot);

    // ?꾩옱 濡쒕퉬???깅줉??PlayerSlot??
    UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Lobby", meta = (AllowPrivateAccess = "true"))
    TArray<UPlayerSlotWidget*> PlayerSlotWidgets;

    // ?좏깮???ㅽ뀒?댁? ?대쫫
    FName SelectedStageName;
};
