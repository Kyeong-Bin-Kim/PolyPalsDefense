// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputConfig.generated.h"

/**
 * 
 */
UCLASS()
class POLYPALSDEFENSE_API UInputConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMapping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> InputTest;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> InputClick;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> InputRightClick;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> InputTower1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> InputTower2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> InputTower3;

};
