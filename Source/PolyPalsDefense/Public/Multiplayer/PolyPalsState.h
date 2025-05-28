#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "PolyPalsState.generated.h"

// 게임 오버 이벤트 델리게이트 (클라용)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameOver);

// 모든 플레이어 준비 완료 이벤트 델리게이트 (클라용)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllPlayersReady);

UCLASS()
class POLYPALSDEFENSE_API APolyPalsState : public AGameStateBase
{
    GENERATED_BODY()

public:
    APolyPalsState();

    // 게임 오버 이벤트 (UI, 클라이언트 연동용)
    UPROPERTY(BlueprintAssignable, Category = "Game")
    FOnGameOver OnGameOver;

    // 모든 플레이어 준비 완료 이벤트
    UPROPERTY(BlueprintAssignable, Category = "Game")
    FOnAllPlayersReady OnAllPlayersReady;

    // 현재 접속 중인 플레이어 수 조회
    UFUNCTION(BlueprintPure, Category = "Lobby")
    int32 GetConnectedPlayers() const { return ConnectedPlayers; }

    // 준비 완료한 플레이어 수 조회
    UFUNCTION(BlueprintPure, Category = "Lobby")
    int32 GetReadyPlayers() const { return ReadyPlayers; }

    // 현재 선택된 스테이지 이름 조회
    UFUNCTION(BlueprintPure, Category = "Lobby")
    FName GetSelectedStage() const { return SelectedStage; }

    UFUNCTION(BlueprintPure, Category = "Game")
    int32 GetRound() const { return CurrentRound; }

    UFUNCTION(BlueprintPure, Category = "Game")
    int32 GetTotalRound() const { return TotalRound; }

    void SetCurrentRound(int32 NewRound);

    // 게임 오버 여부 조회
    UFUNCTION(BlueprintPure, Category = "Game")
    bool IsGameOver() const { return bIsGameOver; }

    // 서버에서 게임 오버 상태 설정
    UFUNCTION(BlueprintCallable, Category = "Game")
    void SetGameOver();

    // 서버에서 접속 플레이어 수 업데이트
    void UpdateConnectedPlayers(int32 Count);

    // 서버에서 준비 완료 플레이어 수 계산 및 갱신
    void UpdateReadyPlayers();

    // 서버에서 선택된 스테이지 설정
    void SetSelectedStage(FName Stage);

protected:
    // 복제할 프로퍼티 등록
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // 접속자 수 변경 시 클라이언트에서 호출
    UFUNCTION()
    void OnRep_ConnectedPlayers();

    // 준비 완료 수 변경 시 클라이언트에서 호출
    UFUNCTION()
    void OnRep_ReadyPlayers();

    // 선택된 스테이지 변경 시 클라이언트에서 호출
    UFUNCTION()
    void OnRep_SelectedStage();

    UFUNCTION()
    void OnRep_CurrentRound();

    // 게임 오버 상태 변경 시 클라이언트에서 호출
    UFUNCTION()
    void OnRep_GameOver();

private:
    // 접속자 수 (Replicated)
    UPROPERTY(ReplicatedUsing = OnRep_ConnectedPlayers)
    int32 ConnectedPlayers;

    // 준비 완료한 플레이어 수 (Replicated)
    UPROPERTY(ReplicatedUsing = OnRep_ReadyPlayers)
    int32 ReadyPlayers;

    // 선택된 스테이지 이름 (Replicated)
    UPROPERTY(ReplicatedUsing = OnRep_SelectedStage)
    FName SelectedStage;

    // 게임 오버 여부 (Replicated)
    UPROPERTY(ReplicatedUsing = OnRep_GameOver)
    bool bIsGameOver;

protected:
    // 현재 라운드 (Replicated)
    UPROPERTY(ReplicatedUsing = OnRep_CurrentRound)
    int32 CurrentRound = 1;

    // 총 라운드 수
    UPROPERTY(EditDefaultsOnly, Category = "Game")
    int32 TotalRound = 10;
};
