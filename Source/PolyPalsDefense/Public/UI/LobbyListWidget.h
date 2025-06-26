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
    virtual void NativeDestruct() override;

    void PopulateLobbyList(const TArray<FLobbyInfo>& LobbyInfos);

protected:
    UPROPERTY(meta = (BindWidget))
    class UVerticalBox* LobbySlotContainer;

    UPROPERTY(meta = (BindWidget))
    class UEditableTextBox* SearchBox;

    UPROPERTY(meta = (BindWidget))
    class UButton* SearchButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* ExitButton;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<ULobbySlotWidget> LobbySlotClass;

    UFUNCTION()
    void HandleJoinLobby(const FString& LobbyID);

    UFUNCTION()
    void HandleSearchClicked();

    UFUNCTION()
    void OnExitGameClicked();

private:
    TArray<FLobbyInfo> CachedLobbies;
    void ApplySearchFilter();

    FTimerHandle RefreshTimerHandle;
    void RefreshSessions();
};
