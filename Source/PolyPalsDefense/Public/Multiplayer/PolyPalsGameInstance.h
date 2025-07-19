#pragma once
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "LobbyInfo.h"
#include "PolyPalsGameInstance.generated.h"

class IOnlineSubsystem;

UCLASS()
class POLYPALSDEFENSE_API UPolyPalsGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    virtual void Init() override;

    void LoginToSteam();

public:
    int32 GetMaxPlayerCount() const { return MaxPlayerCount; }
    void SetMaxPlayerCount(int32 InCount) { MaxPlayerCount = InCount; }

    FString GetPendingLobbyName() const { return PendingLobbyName; }
    void SetPendingLobbyName(const FString& InLobbyName) { PendingLobbyName = InLobbyName; }

	FName GetPendingStageName() const { return PendingStageName; }
    void SetPendingStageName(FName InStageName) { PendingStageName = InStageName; }

private:
    int32 MaxPlayerCount = 4;

    FName PendingStageName;
    FString PendingLobbyName;

private:
    FDelegateHandle OnLoginCompleteHandle;

private:
    void OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);

private:
    IOnlineSubsystem* OnlineSubsystem = nullptr;
    IOnlineIdentityPtr IdentityInterface;
};