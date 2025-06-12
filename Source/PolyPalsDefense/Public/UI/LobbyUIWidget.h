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

    // 외부에서 선택된 스테이지 설정
    UFUNCTION(BlueprintCallable)
    void SetSelectedStage(FName InStageName);

    UFUNCTION(BlueprintCallable)
    void SetRoomTitle(const FString& InRoomTitle);

    // 외부에서 로비 상태 업데이트 (플레이어 수, 준비 수, 스테이지, 방장 여부)
    UFUNCTION(BlueprintCallable)
    void UpdateLobbyInfo(int32 ConnectedPlayers, int32 ReadyPlayers, FName CurrentStage, bool bIsHost);

    void UpdatePlayerSlotReadyState(APolyPalsPlayerState* ChangedState, bool bNewReady);

    // PlayerArray 정보를 이용해 슬롯 UI를 갱신
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

    // 현재 로비에 등록된 PlayerSlot들
    UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Lobby", meta = (AllowPrivateAccess = "true"))
    TArray<UPlayerSlotWidget*> PlayerSlotWidgets;

    // 선택된 스테이지 이름
    FName SelectedStageName;
};
