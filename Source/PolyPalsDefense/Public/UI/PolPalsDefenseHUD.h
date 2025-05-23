#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PolPalsDefenseHUD.generated.h"

UCLASS()
class POLYPALSDEFENSE_API UPolPalsDefenseHUD : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void SetHealth(int32 HP);

    UFUNCTION(BlueprintCallable)
    void SetGold(int32 Amount);

    UFUNCTION(BlueprintCallable)
    void SetRound(int32 Current, int32 Total);

protected:
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* HealthText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* GoldText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* RoundText;
};
