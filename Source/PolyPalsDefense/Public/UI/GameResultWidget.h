#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameResultWidget.generated.h"

class UTextBlock;

UCLASS()
class POLYPALSDEFENSE_API UGameResultWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable)
    void SetResultText(const FText& InText);
protected:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* ResultText;
};