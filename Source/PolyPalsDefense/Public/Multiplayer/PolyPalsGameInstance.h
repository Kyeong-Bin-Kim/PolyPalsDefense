#pragma once
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "LobbyInfo.h"
#include "PolyPalsGameInstance.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSessionsFound, const TArray<FLobbyInfo>&);

class IOnlineSubsystem;

UCLASS()
class POLYPALSDEFENSE_API UPolyPalsGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    virtual void Init() override;

    void LoginToSteam();
    void CreateSteamSession();

    void FindSteamSessions();
    void JoinSteamSession(const FString& LobbyID);

    FOnSessionsFound OnSessionsFound;

public:
    int32 GetMaxPlayerCount() const { return MaxPlayerCount; }
    void SetMaxPlayerCount(int32 InCount) { MaxPlayerCount = InCount; }

private:
    int32 MaxPlayerCount = 4;

private:
    void OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);
    void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
    void CreateSteamSession_Internal();
    void OnDestroySessionForCreateComplete(FName SessionName, bool bWasSuccessful);
    void OnFindSessionsComplete(bool bWasSuccessful);
    void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
    void PerformJoinSession(const FString& LobbyID);
    void OnDestroySessionForJoinComplete(FName SessionName, bool bWasSuccessful);

    FDelegateHandle OnDestroySessionForCreateHandle;

private:
    IOnlineSubsystem* OnlineSubsystem = nullptr;
    IOnlineIdentityPtr IdentityInterface;
    IOnlineSessionPtr SessionInterface;
    TSharedPtr<class FOnlineSessionSearch> SessionSearch;

    FDelegateHandle OnLoginCompleteHandle;
    FDelegateHandle OnCreateSessionCompleteHandle;
    FDelegateHandle OnFindSessionsCompleteHandle;
    FDelegateHandle OnJoinSessionCompleteHandle;
    FDelegateHandle OnDestroySessionCompleteHandle;

    FString PendingJoinSessionId;
};