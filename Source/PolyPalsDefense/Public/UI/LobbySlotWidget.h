#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbySlotWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnJoinLobbyClicked, const FString&, LobbyID);

UCLASS()
class POLYPALSDEFENSE_API ULobbySlotWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void SetupSlot(const FString& InLobbyID, const FString& InLobbyName, int32 InCurrentPlayers, int32 InMaxPlayers);

    UPROPERTY(BlueprintAssignable)
    FOnJoinLobbyClicked OnJoinLobbyClicked;

protected:
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* LobbyNameText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* PlayerCountText;

    UPROPERTY(meta = (BindWidget))
    class UButton* JoinButton;

    FString LobbyID;

    virtual void NativeConstruct() override;

    UFUNCTION()
    void HandleJoinClicked();
};
