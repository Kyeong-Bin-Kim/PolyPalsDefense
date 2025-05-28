#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PolyPalsPlayerState.generated.h"

UCLASS()
class POLYPALSDEFENSE_API APolyPalsPlayerState : public APlayerState
{
    GENERATED_BODY()

public:
    // Ready 상태 설정
    void SetReady(bool bReady);

    // Ready 상태 조회
    bool IsReady() const { return bIsReady; }

    // 개인 골드 추가
    void AddGold(int32 Amount);

    // 현재 개인 골드 조회
    int32 GetPlayerGold() const { return PlayerGold; }

protected:
    // Ready 상태 (Replicated)
    UPROPERTY(Replicated)
    bool bIsReady = false;

    // 개인 골드 (Replicated)
    UPROPERTY(Replicated)
    int32 PlayerGold = 0;

    // Replication 등록
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
