#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TowerLevelWidget.generated.h"

class UImage;
class UTexture2D;

UCLASS()
class POLYPALSDEFENSE_API UTowerLevelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void UpdateLevelUI(int32 CurrentLevel);

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Star1Image;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Star2Image;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Star3Image;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TObjectPtr<UTexture2D> FilledStarTexture;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TObjectPtr<UTexture2D> EmptyStarTexture;
};
