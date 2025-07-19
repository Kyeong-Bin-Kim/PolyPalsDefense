#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PolyPalsPlayerState.generated.h"

UCLASS()
class POLYPALSDEFENSE_API APolyPalsPlayerState : public APlayerState
{
    GENERATED_BODY()

public:

	virtual void BeginPlay() override;

    void SetReadyState(bool bReady);

    bool IsReady() const { return bIsReady; }

    void SetSlotIndex(int32 Index);

    int32 GetSlotIndex() const { return SlotIndex; }

    void AddGold(int32 Amount);

    void SetInitialGold(int32 Amount);

    int32 GetPlayerGold() const { return PlayerGold; }

    UFUNCTION()
    void OnRep_SlotIndex();

    UFUNCTION()
    void OnRep_IsReady();

    UFUNCTION()
    void OnRep_PlayerGold();

protected:
    UPROPERTY(ReplicatedUsing = OnRep_IsReady)
    bool bIsReady = false;

    UPROPERTY(ReplicatedUsing = OnRep_SlotIndex)
    int32 SlotIndex = -1;

    UPROPERTY(ReplicatedUsing = OnRep_PlayerGold)
    int32 PlayerGold = 0;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    bool SpendGold(int32 Amount);
};
