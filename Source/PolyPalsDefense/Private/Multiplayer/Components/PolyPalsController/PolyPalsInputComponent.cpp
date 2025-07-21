#include "Multiplayer/Components/PolyPalsController/PolyPalsInputComponent.h"
#include "Multiplayer/PolyPalsController.h"
#include "Multiplayer/InputConfig.h"
#include "Tower/PreviewBuilding.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputSubsystems.h"

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

    // �������� ������ ���� �ε�
    if (!InputConfig && InputConfigAsset.IsValid())
    {
        InputConfig = InputConfigAsset.LoadSynchronous();
    }

    if (!InputConfig)
    {
        UE_LOG(LogTemp, Error, TEXT("PolyPalsInputComponent: InputConfigAsset not set on %s"), *GetName());
        return;
    }

    // ���� ���ؽ�Ʈ ���
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(InController->GetLocalPlayer()))
    {
        Subsystem->ClearAllMappings();
        Subsystem->AddMappingContext(InputConfig->DefaultMapping, 0);
    }

    // �׼� ���ε�
    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InController->InputComponent))
    {
        // ��Ŭ��
        EIC->BindAction(InputConfig->InputLeftClick, ETriggerEvent::Started, this, &UPolyPalsInputComponent::InputLeftClick);

        // ��Ŭ��
        EIC->BindAction(InputConfig->InputRightClick, ETriggerEvent::Started, this, &UPolyPalsInputComponent::InputRightClick);

        // 1,2,3 Ű
        EIC->BindAction(InputConfig->InputTower1, ETriggerEvent::Started, this, &UPolyPalsInputComponent::InputTower1);

        if (ensure(InputConfig->InputTower1))
        {
            EIC->BindAction(InputConfig->InputTower1, ETriggerEvent::Started,
                this, &UPolyPalsInputComponent::InputTower1);
            UE_LOG(LogTemp, Warning,
                TEXT("[SetupEnhancedInput] Bound InputTower1 action: %s"),
                *InputConfig->InputTower1->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("[SetupEnhancedInput] InputTower1 is NULL!"));
        }

        EIC->BindAction(InputConfig->InputTower2, ETriggerEvent::Started, this, &UPolyPalsInputComponent::InputTower2);
        EIC->BindAction(InputConfig->InputTower3, ETriggerEvent::Started, this, &UPolyPalsInputComponent::InputTower3);
    }
}

void UPolyPalsInputComponent::InputLeftClick(const FInputActionValue& Value)
{
    UE_LOG(LogTemp, Warning, TEXT("[UPolyPalsInputComponent] InputLeftClick)"));

	OnInputLeftClick.Broadcast();
}

void UPolyPalsInputComponent::InputRightClick(const FInputActionValue& Value)
{
	OnInputRightClick.ExecuteIfBound();
}

void UPolyPalsInputComponent::InputTower1(const FInputActionValue& Value)  
{  
    bool bIsPressed = Value.Get<bool>();  

    UE_LOG(LogTemp, Warning, TEXT("[InputTower1] ȣ���! Value=%s"), bIsPressed ? TEXT("Pressed") : TEXT("Released"));  

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