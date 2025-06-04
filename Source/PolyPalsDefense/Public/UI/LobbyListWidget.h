#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyListWidget.generated.h"

class ULobbySlotWidget;

UCLASS()
class POLYPALSDEFENSE_API ULobbyListWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    void PopulateLobbyList(const TArray<FString>& LobbyIDs);

protected:
    UPROPERTY(meta = (BindWidget))
    class UVerticalBox* LobbySlotContainer;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<ULobbySlotWidget> LobbySlotClass;

    UFUNCTION()
    void HandleJoinLobby(const FString& LobbyID);
};
