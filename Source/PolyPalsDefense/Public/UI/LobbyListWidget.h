#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyInfo.h"
#include "LobbyListWidget.generated.h"

class ULobbySlotWidget;

UCLASS()
class POLYPALSDEFENSE_API ULobbyListWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    void PopulateLobbyList(const TArray<FLobbyInfo>& LobbyInfos);

protected:
    UPROPERTY(meta = (BindWidget))
    class UVerticalBox* LobbySlotContainer;

    UPROPERTY(meta = (BindWidget))
    class UEditableTextBox* SearchBox;

    UPROPERTY(meta = (BindWidget))
    class UButton* SearchButton;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<ULobbySlotWidget> LobbySlotClass;

    UFUNCTION()
    void HandleJoinLobby(const FString& LobbyID);

    UFUNCTION()
    void HandleSearchClicked();

private:
    TArray<FLobbyInfo> CachedLobbies;
    void ApplySearchFilter();
};
