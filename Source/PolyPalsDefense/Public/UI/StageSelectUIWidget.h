// StageSelectUIWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StageSelectUIWidget.generated.h"

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
    class UButton* Stage1;

    UPROPERTY(meta = (BindWidget))
    class UButton* Stage2;

    UPROPERTY(meta = (BindWidget))
    class UButton* Stage3;

    UFUNCTION()
    void OnExitGameClicked();

    UFUNCTION()
    void OnEasyClicked();

    UFUNCTION()
    void OnNormalClicked();

    UFUNCTION()
    void OnHardClicked();

    void OnStageSelected(FName StageName);
    void OpenLobbyUI();

    FName LastSelectedStage;
};
