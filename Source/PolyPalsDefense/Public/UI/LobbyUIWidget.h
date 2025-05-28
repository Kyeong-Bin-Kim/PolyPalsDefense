#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerSlotWidget.h"
#include "Components/TextBlock.h"
#include "LobbyUIWidget.generated.h"

UCLASS()
class POLYPALSDEFENSE_API ULobbyUIWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    // 외부에서 선택된 스테이지 설정
    UFUNCTION(BlueprintCallable)
    void SetSelectedStage(FName InStageName);

    // 외부에서 로비 상태 업데이트 (플레이어 수, 준비 수, 스테이지, 방장 여부)
    UFUNCTION(BlueprintCallable)
    void UpdateLobbyInfo(int32 ConnectedPlayers, int32 ReadyPlayers, FName CurrentStage, bool bIsHost);

    // 모든 PlayerSlot Ready 버튼 비활성화
    void DisableAllReadyButtons();

protected:
    UPROPERTY(meta = (BindWidget))
    class UButton* ExitGame;

    UPROPERTY(meta = (BindWidget))
    class UButton* StartGame;

    UPROPERTY(meta = (BindWidget))
    class UVerticalBox* PlayerSlotBox;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Stage;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TotalReadyText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* MaxPlayerCountText;

    UFUNCTION()
    void OnExitGameClicked();

    UFUNCTION()
    void OnStartGameClicked();

    UFUNCTION()
    void HandleSlotReadyClicked(UPlayerSlotWidget* ClickedSlot);

    // 현재 로비에 등록된 PlayerSlot들
    TArray<UPlayerSlotWidget*> PlayerSlotWidgets;

    // 선택된 스테이지 이름
    FName SelectedStageName;
};
