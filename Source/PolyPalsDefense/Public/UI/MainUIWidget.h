#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainUIWidget.generated.h"

class UIPInputWidget;

UCLASS(Blueprintable)
class POLYPALSDEFENSE_API UMainUIWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    // ?몃??먯꽌 ?됰꽕???먮뒗 Steam ID瑜??ㅼ젙
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

    // 異뷀썑 異붽? ?덉젙
    //UPROPERTY(meta = (BindWidget))
    //class UButton* Settings;

    // Bindable Panel
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UVerticalBox* MultiplayerButtons;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* PlayerNameText;

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

    // Bindable Menu State
    bool bIsMultiplayerMenuOpen = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI|IP")
    TSubclassOf<UIPInputWidget> IPInputWidgetClass;

private:
    FButtonStyle CachedStartButtonStyle;
};