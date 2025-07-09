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

    // Ready ?곹깭 ?ㅼ젙
    void SetReadyState(bool bReady);

    // Ready ?곹깭 議고쉶
    bool IsReady() const { return bIsReady; }

	// Slot Index ?ㅼ젙 諛?議고쉶
    void SetSlotIndex(int32 Index);

    int32 GetSlotIndex() const { return SlotIndex; }

    // 媛쒖씤 怨⑤뱶 異붽?
    void AddGold(int32 Amount);

    // 珥덇린 怨⑤뱶 ?ㅼ젙
    void SetInitialGold(int32 Amount);

    // ?꾩옱 媛쒖씤 怨⑤뱶 議고쉶
    int32 GetPlayerGold() const { return PlayerGold; }

    UFUNCTION()
    void OnRep_SlotIndex();

    UFUNCTION()
    void OnRep_IsReady();

    UFUNCTION()
    void OnRep_PlayerGold();

protected:
    // Ready ?곹깭
    UPROPERTY(ReplicatedUsing = OnRep_IsReady)
    bool bIsReady = false;

    UPROPERTY(ReplicatedUsing = OnRep_SlotIndex)
    int32 SlotIndex = -1;

    // 媛쒖씤 怨⑤뱶
    UPROPERTY(ReplicatedUsing = OnRep_PlayerGold)
    int32 PlayerGold = 0;

    // Replication ?깅줉
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    bool SpendGold(int32 Amount);
};
