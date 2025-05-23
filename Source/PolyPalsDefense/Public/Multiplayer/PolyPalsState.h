#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "PolyPalsState.generated.h"

// 게임 오버 이벤트 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameOver);

// 모든 플레이어가 준비 완료되면 발생하는 이벤트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllPlayersReady);

UCLASS()
class POLYPALSDEFENSE_API APolyPalsState : public AGameStateBase
{
    GENERATED_BODY()

public:
    APolyPalsState();

    // 게임 오버 설정 시 발생하는 이벤트
    UPROPERTY(BlueprintAssignable, Category = "Game")
    FOnGameOver OnGameOver;

    // 모든 플레이어 준비 완료 이벤트
    UPROPERTY(BlueprintAssignable, Category = "Game")
    FOnAllPlayersReady OnAllPlayersReady;

    // 현재 골드 조회
    UFUNCTION(BlueprintPure, Category = "Player")
    int32 GetGold() const { return Gold; }

    // 서버에서 골드 추가
    UFUNCTION(Server, Reliable)
    void AddGold(int32 Amount);

    // 게임 오버가 발생했는지 여부 반환
    UFUNCTION(BlueprintPure, Category = "Game")
    bool IsGameOver() const { return bIsGameOver; }

    // GameMode에서 게임 오버를 트리거할 때 호출
    UFUNCTION(BlueprintCallable, Category = "Game")
    void SetGameOver();

protected:
    // 복제할 프로퍼티 등록
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // 골드 변경 시 UI에 알림
    UFUNCTION()
    void OnRep_Gold();

    // GameOver 플래그 동기화 시 호출되어 이벤트 브로드캐스트
    UFUNCTION()
    void OnRep_GameOver();

private:
    // 골드 값 (클라이언트 UI용 복제 - 플레이어별 골드)
    UPROPERTY(ReplicatedUsing = OnRep_Gold)
    int32 Gold;

    // 게임 오버 상태 여부 (ReplicatedUsing을 통해 OnRep 호출)
    UPROPERTY(ReplicatedUsing = OnRep_GameOver)
    bool bIsGameOver;
};
