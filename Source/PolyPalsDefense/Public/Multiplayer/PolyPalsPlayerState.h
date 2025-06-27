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

    // Ready 상태 설정
    void SetReadyState(bool bReady);

    // Ready 상태 조회
    bool IsReady() const { return bIsReady; }

	// Slot Index 설정 및 조회
    void SetSlotIndex(int32 Index);

    int32 GetSlotIndex() const { return SlotIndex; }

    // 개인 골드 추가
    void AddGold(int32 Amount);

    // 초기 골드 설정
    void SetInitialGold(int32 Amount);

    // 현재 개인 골드 조회
    int32 GetPlayerGold() const { return PlayerGold; }

    UFUNCTION()
    void OnRep_IsReady();

    UFUNCTION()
    void OnRep_PlayerGold();

protected:
    // Ready 상태 (Replicated)
    UPROPERTY(Replicated)
    bool bIsReady = false;

    UPROPERTY(Replicated)
    int32 SlotIndex = -1;

    // 개인 골드 (Replicated)
    UPROPERTY(ReplicatedUsing = OnRep_PlayerGold)
    int32 PlayerGold = 0;

    // Replication 등록
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    bool SpendGold(int32 Amount);
};
