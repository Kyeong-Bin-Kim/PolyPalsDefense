#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyHealthBarWidget.generated.h"

class UProgressBar;

UCLASS()
class POLYPALSDEFENSE_API UEnemyHealthBarWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void SetHealthBarRatio(float Ratio);

protected:
    UPROPERTY(meta = (BindWidget))
    UProgressBar* HealthProgressBar;
};
