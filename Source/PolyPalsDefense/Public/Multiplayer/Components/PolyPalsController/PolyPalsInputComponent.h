// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InputActionValue.h"
#include "PolyPalsInputComponent.generated.h"

DECLARE_DELEGATE(FOnInputTest)
DECLARE_DELEGATE(FOnInputClick)

class APolyPalsController;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POLYPALSDEFENSE_API UPolyPalsInputComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPolyPalsInputComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void SetupEnhancedInput(APolyPalsController* const InController);

private:
	
	void InputTest(const FInputActionValue& Value);
	void InputClick(const FInputActionValue& Value);


public:
	FOnInputTest OnInputTest;
	FOnInputClick OnInputClick;

protected:
	UPROPERTY()
	TObjectPtr<class UInputConfig> InputConfig;

private:	
	UPROPERTY()
	TObjectPtr<APolyPalsController> PolyPalsController;


		
};
