#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "PolyPalsState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameOver);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameClear);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllPlayersReady);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLobbyCountdownStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLobbyCountdownUpdated, int32, SecondsRemaining);

UCLASS()
class POLYPALSDEFENSE_API APolyPalsState : public AGameStateBase
{
    GENERATED_BODY()

public:
    APolyPalsState();

    UPROPERTY(BlueprintAssignable, Category = "Game")
    FOnAllPlayersReady OnAllPlayersReady;

    UPROPERTY(BlueprintAssignable, Category = "Lobby")
    FOnLobbyCountdownStarted OnLobbyCountdownStarted;

    UPROPERTY(BlueprintAssignable, Category = "Lobby")
    FOnLobbyCountdownUpdated OnLobbyCountdownUpdated;

    UPROPERTY(BlueprintAssignable, Category = "Game")
    FOnGameOver OnGameOver;

    UPROPERTY(BlueprintAssignable, Category = "Game")
    FOnGameClear OnGameClear;

    UFUNCTION(BlueprintPure, Category = "Lobby")
    int32 GetConnectedPlayers() const { return ConnectedPlayers; }

    UFUNCTION(BlueprintPure, Category = "Lobby")
    int32 GetReadyPlayers() const { return ReadyPlayers; }

    UFUNCTION(BlueprintPure, Category = "Lobby")
    FName GetSelectedStage() const { return SelectedStage; }

    UFUNCTION(BlueprintPure, Category = "Game")
    int32 GetRound() const { return CurrentRound; }

    UFUNCTION(BlueprintPure, Category = "Game")
    int32 GetTotalRound() const { return TotalRound; }

    void SetCurrentRound(int32 NewRound);

    UFUNCTION(BlueprintPure, Category = "Game")
    bool IsGameOver() const { return bIsGameOver; }

    UFUNCTION(BlueprintPure, Category = "Game")
    bool IsGameClear() const { return bIsGameClear; }

    UFUNCTION(BlueprintCallable, Category = "Game")
    void SetGameOver();

    UFUNCTION(BlueprintCallable, Category = "Game")
    void SetGameClear();

    UFUNCTION(BlueprintPure, Category = "Lobby")
    FString GetLobbyName() const { return LobbyName; }

    void UpdateConnectedPlayers(int32 Count);

    void UpdateReadyPlayers();

    void SetSelectedStage(FName Stage);

    void SetLobbyName(const FString& Name);

    virtual void AddPlayerState(APlayerState* PlayerState) override;

    virtual void RemovePlayerState(APlayerState* PlayerState) override;

    void StartLobbyCountdown(int32 InSeconds);

    void UpdateLobbyCountdown();

private:
    void NotifyLobbyStateChanged();

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION()
    void OnRep_ConnectedPlayers();

    UFUNCTION()
    void OnRep_SelectedStage();

    UFUNCTION()
    void OnRep_LobbyName();

    UFUNCTION()
    void OnRep_ReadyPlayers();

    UFUNCTION()
    void OnRep_CurrentRound();

    UFUNCTION()
    void OnRep_LobbyCountdown();

    UFUNCTION()
    void OnRep_GameOver();

    UFUNCTION()
    void OnRep_GameClear();

private:
    UPROPERTY(ReplicatedUsing = OnRep_ConnectedPlayers)
    int32 ConnectedPlayers;

    UPROPERTY(ReplicatedUsing = OnRep_ReadyPlayers)
    int32 ReadyPlayers;

    UPROPERTY(ReplicatedUsing = OnRep_SelectedStage)
    FName SelectedStage;

    UPROPERTY(ReplicatedUsing = OnRep_LobbyName)
    FString LobbyName;

    UPROPERTY(ReplicatedUsing = OnRep_LobbyCountdown)
    int32 LobbyCountdown = 0;

    UPROPERTY(ReplicatedUsing = OnRep_GameOver)
    bool bIsGameOver;

    UPROPERTY(ReplicatedUsing = OnRep_GameClear)
    bool bIsGameClear;

    FTimerHandle LobbyCountdownTimerHandle;

protected:
    UPROPERTY(ReplicatedUsing = OnRep_CurrentRound)
    int32 CurrentRound = 1;

    UPROPERTY(EditDefaultsOnly, Category = "Game")
    int32 TotalRound = 10;
};
