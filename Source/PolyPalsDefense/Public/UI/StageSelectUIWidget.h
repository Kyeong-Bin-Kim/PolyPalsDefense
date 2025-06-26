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
    class UButton* DirtStage;

    UPROPERTY(meta = (BindWidget))
    class UButton* SnowStage;

    UFUNCTION()
    void OnDirtClicked();

    UFUNCTION()
    void OnSnowClicked();

    UFUNCTION()
    void OnExitGameClicked();

    void HandleStageSelected(FName StageName);

    FName LastSelectedStage;
};
