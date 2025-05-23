// LobbyUIWidget.h
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

    /** 스테이지 선택 이름을 외부에서 전달 */
    UFUNCTION(BlueprintCallable)
    void SetSelectedStage(FName InStageName);

protected:
    UPROPERTY(meta = (BindWidget))
    class UButton* ExitGame;

    UPROPERTY(meta = (BindWidget))
    class UButton* StartGame;

    UPROPERTY(meta = (BindWidget))
    class UVerticalBox* PlayerSlotBox;

    /** 블루프린트에 있는 Stage 텍스트 표시용 */
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Stage;

    UFUNCTION()
    void OnExitGameClicked();

    UFUNCTION()
    void OnStartGameClicked();

    UFUNCTION()
    void HandleSlotReadyClicked(UPlayerSlotWidget* ClickedSlot);

    /** 모든 슬롯 */
    TArray<UPlayerSlotWidget*> PlayerSlotWidgets;

    /** 선택된 스테이지 이름 저장 */
    FName SelectedStageName;
};
