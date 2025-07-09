#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "PolyPalsState.generated.h"

// 寃뚯엫 ?ㅻ쾭 ?대깽???몃━寃뚯씠??(?대씪??
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameOver);

// 紐⑤뱺 ?뚮젅?댁뼱 以鍮??꾨즺 ?대깽???몃━寃뚯씠??(?대씪??
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllPlayersReady);

UCLASS()
class POLYPALSDEFENSE_API APolyPalsState : public AGameStateBase
{
    GENERATED_BODY()

public:
    APolyPalsState();

    // 寃뚯엫 ?ㅻ쾭 ?대깽??(UI, ?대씪?댁뼵???곕룞??
    UPROPERTY(BlueprintAssignable, Category = "Game")
    FOnGameOver OnGameOver;

    // 紐⑤뱺 ?뚮젅?댁뼱 以鍮??꾨즺 ?대깽??
    UPROPERTY(BlueprintAssignable, Category = "Game")
    FOnAllPlayersReady OnAllPlayersReady;

    // ?꾩옱 ?묒냽 以묒씤 ?뚮젅?댁뼱 ??議고쉶
    UFUNCTION(BlueprintPure, Category = "Lobby")
    int32 GetConnectedPlayers() const { return ConnectedPlayers; }

    // 以鍮??꾨즺???뚮젅?댁뼱 ??議고쉶
    UFUNCTION(BlueprintPure, Category = "Lobby")
    int32 GetReadyPlayers() const { return ReadyPlayers; }

    // ?꾩옱 ?좏깮???ㅽ뀒?댁? ?대쫫 議고쉶
    UFUNCTION(BlueprintPure, Category = "Lobby")
    FName GetSelectedStage() const { return SelectedStage; }

    UFUNCTION(BlueprintPure, Category = "Game")
    int32 GetRound() const { return CurrentRound; }

    UFUNCTION(BlueprintPure, Category = "Game")
    int32 GetTotalRound() const { return TotalRound; }

    void SetCurrentRound(int32 NewRound);

    // 寃뚯엫 ?ㅻ쾭 ?щ? 議고쉶
    UFUNCTION(BlueprintPure, Category = "Game")
    bool IsGameOver() const { return bIsGameOver; }

    // ?쒕쾭?먯꽌 寃뚯엫 ?ㅻ쾭 ?곹깭 ?ㅼ젙
    UFUNCTION(BlueprintCallable, Category = "Game")
    void SetGameOver();

    // ?쒕쾭?먯꽌 ?묒냽 ?뚮젅?댁뼱 ???낅뜲?댄듃
    void UpdateConnectedPlayers(int32 Count);

    // ?쒕쾭?먯꽌 以鍮??꾨즺 ?뚮젅?댁뼱 ??怨꾩궛 諛?媛깆떊
    void UpdateReadyPlayers();

    // ?쒕쾭?먯꽌 ?좏깮???ㅽ뀒?댁? ?ㅼ젙
    void SetSelectedStage(FName Stage);

    // ?쒕쾭?먯꽌 濡쒕퉬 ?대쫫 ?ㅼ젙
    void SetLobbyName(const FString& Name);

    UFUNCTION(BlueprintPure, Category = "Lobby")
    FString GetLobbyName() const { return LobbyName; }

    // ?뚮젅?댁뼱 ?곹깭 諛곗뿴??媛깆떊????濡쒕퉬 UI 媛깆떊???꾪빐 ?몄텧
    virtual void AddPlayerState(APlayerState* PlayerState) override;

    virtual void RemovePlayerState(APlayerState* PlayerState) override;

private:
    void NotifyLobbyStateChanged();

protected:
    // 蹂듭젣???꾨줈?쇳떚 ?깅줉
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // ?묒냽????蹂寃????대씪?댁뼵?몄뿉???몄텧
    UFUNCTION()
    void OnRep_ConnectedPlayers();

    // 以鍮??꾨즺 ??蹂寃????대씪?댁뼵?몄뿉???몄텧
    UFUNCTION()
    void OnRep_ReadyPlayers();

    // ?좏깮???ㅽ뀒?댁? 蹂寃????대씪?댁뼵?몄뿉???몄텧
    UFUNCTION()
    void OnRep_SelectedStage();

    UFUNCTION()
    void OnRep_LobbyName();

    UFUNCTION()
    void OnRep_CurrentRound();

    // 寃뚯엫 ?ㅻ쾭 ?곹깭 蹂寃????대씪?댁뼵?몄뿉???몄텧
    UFUNCTION()
    void OnRep_GameOver();

private:
    // ?묒냽????(Replicated)
    UPROPERTY(ReplicatedUsing = OnRep_ConnectedPlayers)
    int32 ConnectedPlayers;

    // 以鍮??꾨즺???뚮젅?댁뼱 ??(Replicated)
    UPROPERTY(ReplicatedUsing = OnRep_ReadyPlayers)
    int32 ReadyPlayers;

    // ?몄뒪?멸? ?좏깮???ㅽ뀒?댁? ?대쫫 (Replicated)
    UPROPERTY(ReplicatedUsing = OnRep_SelectedStage)
    FName SelectedStage;

    // ?몄뒪?멸? ?ㅼ젙??濡쒕퉬 ?대쫫 (Replicated)
    UPROPERTY(ReplicatedUsing = OnRep_LobbyName)
    FString LobbyName;

    // 寃뚯엫 ?ㅻ쾭 ?щ? (Replicated)
    UPROPERTY(ReplicatedUsing = OnRep_GameOver)
    bool bIsGameOver;

protected:
    // ?꾩옱 ?쇱슫??(Replicated)
    UPROPERTY(ReplicatedUsing = OnRep_CurrentRound)
    int32 CurrentRound = 1;

    // 珥??쇱슫????
    UPROPERTY(EditDefaultsOnly, Category = "Game")
    int32 TotalRound = 10;
};
