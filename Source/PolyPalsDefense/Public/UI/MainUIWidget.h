#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainUIWidget.generated.h"

class UStageSelectUIWidget;

UCLASS()
class POLYPALSDEFENSE_API UMainUIWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    // 외부에서 닉네임 또는 Steam ID를 설정
    UFUNCTION(BlueprintCallable)
    void SetPlayerNameText(const FString& Name);

protected:
    // Bindable Buttons
    UPROPERTY(meta = (BindWidget))
    class UButton* StartGame;

    UPROPERTY(meta = (BindWidget))
    class UButton* ExitGame;

    UPROPERTY(meta = (BindWidget))
    class UButton* CreateRoom;

    UPROPERTY(meta = (BindWidget))
    class UButton* SearchRoom;

    // 추후 추가 예정
    //UPROPERTY(meta = (BindWidget))
    //class UButton* Settings;

    // Bindable Panel
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UVerticalBox* MultiplayerButtons;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* PlayerNameText;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UStageSelectUIWidget> StageSelectWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class ULobbyUIWidget> LobbyUIWidgetClass;

    // Functions
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

    UFUNCTION()
    void OnExitGameClicked();

    UFUNCTION()
    void OnSettingsClicked();

    UFUNCTION()
    void OnStartGameClicked();

    UFUNCTION()
    void OnCreateRoomClicked();

    UFUNCTION()
    void OnSearchRoomClicked();

    UFUNCTION()
    void HandleStageSelected(FName SelectedStage);

    // Bindable Menu State
    bool bIsMultiplayerMenuOpen = false;

private:
    FButtonStyle CachedStartButtonStyle;
};