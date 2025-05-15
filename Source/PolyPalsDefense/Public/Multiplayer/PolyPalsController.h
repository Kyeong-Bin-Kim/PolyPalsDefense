// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PolyPalsController.generated.h"

/**
 * 
 */
UCLASS()
class POLYPALSDEFENSE_API APolyPalsController : public APlayerController
{
	GENERATED_BODY()
	

public:
	APolyPalsController();


protected:
	virtual void SetupInputComponent() override;

	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class UPolyPalsInputComponent> PolyPalsInputComponent;
};
