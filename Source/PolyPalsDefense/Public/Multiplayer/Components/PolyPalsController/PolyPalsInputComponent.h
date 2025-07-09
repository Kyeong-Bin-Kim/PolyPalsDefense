#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InputActionValue.h"
#include "PolyPalsInputComponent.generated.h"

class UInputConfig;
class APolyPalsController;

DECLARE_MULTICAST_DELEGATE(FOnLeftClick);
DECLARE_DELEGATE(FOnRightClick);
DECLARE_DELEGATE(FOnTower1);
DECLARE_DELEGATE(FOnTower2);
DECLARE_DELEGATE(FOnTower3);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POLYPALSDEFENSE_API UPolyPalsInputComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPolyPalsInputComponent();

public:	
	void SetupEnhancedInput(APolyPalsController* const InController);

	FOnLeftClick   OnInputLeftClick;
	FOnRightClick  OnInputRightClick;
	FOnTower1      OnInputTower1;
	FOnTower2      OnInputTower2;
	FOnTower3      OnInputTower3;

private:
	void InputLeftClick(const FInputActionValue& Value);
	void InputRightClick(const FInputActionValue& Value);
	void InputTower1(const FInputActionValue& Value);
	void InputTower2(const FInputActionValue& Value);
	void InputTower3(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TSoftObjectPtr<UInputConfig> InputConfigAsset;

	UInputConfig* InputConfig = nullptr;

	UPROPERTY()
	TObjectPtr<APolyPalsController> PolyPalsController;
};
