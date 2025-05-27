// Fill out your copyright notice in the Description page of Project Settings.


#include "Multiplayer/Components/PolyPalsController/PolyPalsInputComponent.h"
#include "Multiplayer/PolyPalsController.h"
#include "Multiplayer/InputConfig.h"
#include "Tower/PreviewBuilding.h"

#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UPolyPalsInputComponent::UPolyPalsInputComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	ConstructorHelpers::FObjectFinder<UInputConfig> InputConfigData(TEXT("/Game/Multiplayer/Input/Data_InputConfig.Data_InputConfig"));
	if (InputConfigData.Succeeded())
		InputConfig = InputConfigData.Object;

}


// Called when the game starts
void UPolyPalsInputComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void UPolyPalsInputComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	//Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPolyPalsInputComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UPolyPalsInputComponent::SetupEnhancedInput(APolyPalsController* const InController)
{
	PolyPalsController = InController;
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PolyPalsController->GetLocalPlayer());
	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(InputConfig->DefaultMapping, 0);
	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PolyPalsController->InputComponent);
	if (Input)
	{
		Input->BindAction(InputConfig->InputTest, ETriggerEvent::Started, this, &UPolyPalsInputComponent::InputTest);
		Input->BindAction(InputConfig->InputClick, ETriggerEvent::Started, this, &UPolyPalsInputComponent::InputClick);
		Input->BindAction(InputConfig->InputRightClick, ETriggerEvent::Started, this, &UPolyPalsInputComponent::InputRightClick);
		Input->BindAction(InputConfig->InputTower1, ETriggerEvent::Started, this, &UPolyPalsInputComponent::InputTower1);
		Input->BindAction(InputConfig->InputTower2, ETriggerEvent::Started, this, &UPolyPalsInputComponent::InputTower2);
		Input->BindAction(InputConfig->InputTower3, ETriggerEvent::Started, this, &UPolyPalsInputComponent::InputTower3);
	}
}

void UPolyPalsInputComponent::InputTest(const FInputActionValue& Value)
{
	OnInputTest.ExecuteIfBound();
}

void UPolyPalsInputComponent::InputClick(const FInputActionValue& Value)
{
	OnInputClick.Broadcast();
}

void UPolyPalsInputComponent::InputRightClick(const FInputActionValue& Value)
{
	OnInputRightClick.ExecuteIfBound();
}

void UPolyPalsInputComponent::InputTower1(const FInputActionValue& Value)
{
	OnInputTower1.ExecuteIfBound();
}

void UPolyPalsInputComponent::InputTower2(const FInputActionValue& Value)
{
	OnInputTower2.ExecuteIfBound();
}

void UPolyPalsInputComponent::InputTower3(const FInputActionValue& Value)
{
	OnInputTower3.ExecuteIfBound();
}
