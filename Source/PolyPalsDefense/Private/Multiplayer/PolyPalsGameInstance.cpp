#include "PolyPalsGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

void UPolyPalsGameInstance::Init()
{
    Super::Init();

    OnlineSubsystem = IOnlineSubsystem::Get();
    if (OnlineSubsystem)
    {
        IdentityInterface = OnlineSubsystem->GetIdentityInterface();
        SessionInterface = OnlineSubsystem->GetSessionInterface();

        if (IdentityInterface.IsValid())
        {
            OnLoginCompleteHandle = IdentityInterface->AddOnLoginCompleteDelegate_Handle(0, FOnLoginCompleteDelegate::CreateUObject(this, &UPolyPalsGameInstance::OnLoginComplete));
        }

        if (SessionInterface.IsValid())
        {
            OnCreateSessionCompleteHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(FOnCreateSessionCompleteDelegate::CreateUObject(this, &UPolyPalsGameInstance::OnCreateSessionComplete));
        }
    }

    LoginToSteam();
}

void UPolyPalsGameInstance::LoginToSteam()
{
    if (IdentityInterface.IsValid())
    {
        FOnlineAccountCredentials Credentials;
        IdentityInterface->Login(0, Credentials);
    }
}

void UPolyPalsGameInstance::CreateSteamSession()
{
    if (SessionInterface.IsValid())
    {
        FOnlineSessionSettings Settings;
        Settings.bIsLANMatch = false;
        Settings.NumPublicConnections = 4;
        Settings.bShouldAdvertise = true;
        SessionInterface->CreateSession(0, NAME_GameSession, Settings);
    }
}

void UPolyPalsGameInstance::OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
    UE_LOG(LogTemp, Log, TEXT("Steam login %s"), bWasSuccessful ? TEXT("succeeded") : TEXT("failed"));
    if (bWasSuccessful)
    {
        CreateSteamSession();
    }
}

void UPolyPalsGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    UE_LOG(LogTemp, Log, TEXT("Session creation %s"), bWasSuccessful ? TEXT("succeeded") : TEXT("failed"));
}