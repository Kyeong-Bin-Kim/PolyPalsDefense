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

    UFUNCTION(BlueprintCallable)
    void SetSelectedStage(FName InStageName);

    UFUNCTION(BlueprintCallable)
    void SetRoomTitle(const FString& InRoomTitle);

    UFUNCTION(BlueprintCallable)
    void UpdateLobbyInfo(int32 ConnectedPlayers, int32 ReadyPlayers, FName CurrentStage, bool bIsHost);

    void UpdatePlayerSlotReadyState(APolyPalsPlayerState* ChangedState, bool bNewReady);

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
    class UTextBlock* CountdownText;

    UPROPERTY(meta = (BindWidget))
    class UButton* ExitGame;

    UFUNCTION()
    void OnExitGameClicked();

    UFUNCTION()
    void HandleSlotReadyClicked(UPlayerSlotWidget* ClickedSlot);

    UFUNCTION()
    void HandleLobbyCountdownUpdated(int32 SecondsRemaining);

    UFUNCTION()
    void HandleLobbyCountdownStarted();

    UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Lobby", meta = (AllowPrivateAccess = "true"))
    TArray<UPlayerSlotWidget*> PlayerSlotWidgets;

    FName SelectedStageName;
};
