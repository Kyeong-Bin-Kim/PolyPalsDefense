// LobbyUIWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerSlotWidget.h" 
#include "LobbyUIWidget.generated.h"

UCLASS()
class POLYPALSDEFENSE_API ULobbyUIWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

protected:
    UPROPERTY(meta = (BindWidget))
    class UButton* ExitGame;

    UPROPERTY(meta = (BindWidget))
    class UButton* StartGame;

    UPROPERTY(meta = (BindWidget))
    class UVerticalBox* PlayerSlotBox; // 슬롯들을 담는 컨테이너

    UFUNCTION()
    void OnExitGameClicked();

    UFUNCTION()
    void OnStartGameClicked();

    UFUNCTION()
    void HandleSlotReadyClicked(UPlayerSlotWidget* ClickedSlot);

    TArray<UPlayerSlotWidget*> PlayerSlotWidgets;
};