#pragma once
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "PolyPalsGameInstance.generated.h"

class IOnlineSubsystem;

UCLASS()
class POLYPALSDEFENSE_API UPolyPalsGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    virtual void Init() override;

    void LoginToSteam();
    void CreateSteamSession();

private:
    void OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);
    void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

private:
    IOnlineSubsystem* OnlineSubsystem = nullptr;
    IOnlineIdentityPtr IdentityInterface;
    IOnlineSessionPtr SessionInterface;

    FDelegateHandle OnLoginCompleteHandle;
    FDelegateHandle OnCreateSessionCompleteHandle;
};