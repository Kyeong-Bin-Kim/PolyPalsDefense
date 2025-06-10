#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StageSelectUIWidget.generated.h"

class ULobbyUIWidget;
class UMainUIWidget;

UCLASS()
class POLYPALSDEFENSE_API UStageSelectUIWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

protected:
    UPROPERTY(meta = (BindWidget))
    class UButton* ExitGame;

    UPROPERTY(meta = (BindWidget))
    class UButton* DirtStage;

    UPROPERTY(meta = (BindWidget))
    class UButton* SnowStage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<ULobbyUIWidget> LobbyUIWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UMainUIWidget> MainUIWidgetClass;

    UFUNCTION()
    void OnDirtClicked();

    UFUNCTION()
    void OnSnowClicked();

    UFUNCTION()
    void OnExitGameClicked();

    void HandleStageSelected(FName StageName);
    void OpenLobbyUI();

    FName LastSelectedStage;
};
