#include "Multiplayer/Components/PolyPalsController/PolyPalsInputComponent.h"
#include "Multiplayer/PolyPalsController.h"
#include "Multiplayer/InputConfig.h"
#include "Tower/PreviewBuilding.h"

#include "Kismet/GameplayStatics.h"

UPolyPalsInputComponent::UPolyPalsInputComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	ConstructorHelpers::FObjectFinder<UInputConfig> InputConfigData(TEXT("/Game/Multiplayer/Input/Data_InputConfig.Data_InputConfig"));

	if (InputConfigData.Succeeded())
		InputConfig = InputConfigData.Object;

}

void UPolyPalsInputComponent::SetupEnhancedInput(APolyPalsController* const InController)
{

    UE_LOG(LogTemp, Warning, TEXT("SetupEnhancedInput: Controller.InputComponent class = %s"),
        *InController->InputComponent->GetClass()->GetName());

	PolyPalsController = InController;

    // 참조에서 에셋을 동기 로드
    if (!InputConfig && InputConfigAsset.IsValid())
    {
        InputConfig = InputConfigAsset.LoadSynchronous();
    }

    if (!InputConfig)
    {
        UE_LOG(LogTemp, Error, TEXT("PolyPalsInputComponent: InputConfigAsset not set on %s"), *GetName());
        return;
    }

    // 매핑 컨텍스트 등록
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(InController->GetLocalPlayer()))
    {
        Subsystem->ClearAllMappings();
        Subsystem->AddMappingContext(InputConfig->DefaultMapping, 0);
    }

    // 액션 바인딩
    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InController->InputComponent))
    {
        // 좌클릭
        EIC->BindAction(InputConfig->InputLeftClick, ETriggerEvent::Completed, this, &UPolyPalsInputComponent::InputLeftClick);

        // 우클릭
        EIC->BindAction(InputConfig->InputRightClick, ETriggerEvent::Completed, this, &UPolyPalsInputComponent::InputRightClick);

        // 1,2,3 키
        EIC->BindAction(InputConfig->InputTower1, ETriggerEvent::Completed, this, &UPolyPalsInputComponent::InputTower1);
        EIC->BindAction(InputConfig->InputTower2, ETriggerEvent::Completed, this, &UPolyPalsInputComponent::InputTower2);
        EIC->BindAction(InputConfig->InputTower3, ETriggerEvent::Completed, this, &UPolyPalsInputComponent::InputTower3);
    }
}

void UPolyPalsInputComponent::InputLeftClick(const FInputActionValue& Value)
{
	OnInputLeftClick.Broadcast();
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