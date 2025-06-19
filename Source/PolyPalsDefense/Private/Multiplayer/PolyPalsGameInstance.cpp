#include "PolyPalsGameInstance.h"
#include "PolyPalsController.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"
#include "Kismet/GameplayStatics.h"

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
    if (!SessionInterface.IsValid() || !IdentityInterface.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("세션 인터페이스나 아이덴티티 인터페이스가 유효하지 않습니다."));
        return;
    }

    // 1) 이전에 같은 이름으로 만들어진 세션이 있으면 지우기
    if (SessionInterface->GetNamedSession(NAME_GameSession) != nullptr)
    {
        SessionInterface->DestroySession(NAME_GameSession);
    }

    // 2) OnCreateSessionComplete 바인딩 (콜백에서 StartSession 등 후속 처리)
    SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(
        FOnCreateSessionCompleteDelegate::CreateUObject(this, &UPolyPalsGameInstance::OnCreateSessionComplete)
    );

    // 3) 세션 세팅
    FOnlineSessionSettings Settings;
    Settings.bIsLANMatch = false;
    Settings.NumPublicConnections = 4;
    Settings.bShouldAdvertise = true;
    Settings.bUseLobbiesIfAvailable = true;     // Steam 로비 API 사용
    Settings.bAllowJoinViaPresence = true;     // 프레젠스 조인 허용
    Settings.bUsesPresence = true;     // 프레젠스 자체 사용 플래그 (★필수)
    Settings.bAllowInvites = true;     // 친구 초대 허용

    // 4) 호스트 닉네임 가져오기
    FString PlayerName = TEXT("Host");
    TSharedPtr<const FUniqueNetId> UserId = IdentityInterface->GetUniquePlayerId(0);
    if (UserId.IsValid())
    {
        PlayerName = IdentityInterface->GetPlayerNickname(*UserId);
    }

    // 5) 커스텀 데이터 삽입
    FString LobbyName = FString::Printf(TEXT("%s's Room"), *PlayerName);
    Settings.Set(
        TEXT("LobbyName"),
        LobbyName,
        EOnlineDataAdvertisementType::ViaOnlineService  // 혹은 ViaOnlineServiceAndPing
    );
    Settings.Set(
        TEXT("InProgress"),
        false,
        EOnlineDataAdvertisementType::ViaOnlineService
    );

    // 6) 세션 생성 호출
    SessionInterface->CreateSession(0, NAME_GameSession, Settings);
}

void UPolyPalsGameInstance::OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
    UE_LOG(LogTemp, Log, TEXT("Steam login %s"), bWasSuccessful ? TEXT("succeeded") : TEXT("failed"));
    if (bWasSuccessful)
    {
        FString PlayerName = IdentityInterface.IsValid() ? IdentityInterface->GetPlayerNickname(UserId) : TEXT("");

        if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
        {
            if (APolyPalsController* PPC = Cast<APolyPalsController>(PC))
            {
                PPC->UpdatePlayerNickname(PlayerName);
            }
        }

        CreateSteamSession();
    }
}

void UPolyPalsGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    UE_LOG(LogTemp, Log, TEXT("Session creation %s"), bWasSuccessful ? TEXT("succeeded") : TEXT("failed"));
}

void UPolyPalsGameInstance::FindSteamSessions()
{
    if (SessionInterface.IsValid())
    {
        SessionSearch = MakeShareable(new FOnlineSessionSearch());
        SessionSearch->MaxSearchResults = 100;
        SessionSearch->bIsLanQuery = false;
        SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
        OnFindSessionsCompleteHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FOnFindSessionsCompleteDelegate::CreateUObject(this, &UPolyPalsGameInstance::OnFindSessionsComplete));
        SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
    }
}

void UPolyPalsGameInstance::JoinSteamSession(const FString& LobbyID)
{
    if (!SessionInterface.IsValid() || !SessionSearch.IsValid())
        return;

    const FOnlineSessionSearchResult* Found = SessionSearch->SearchResults.FindByPredicate([&](const FOnlineSessionSearchResult& Result)
        {
            return Result.GetSessionIdStr() == LobbyID;
        });

    if (Found)
    {
        OnJoinSessionCompleteHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(FOnJoinSessionCompleteDelegate::CreateUObject(this, &UPolyPalsGameInstance::OnJoinSessionComplete));
        SessionInterface->JoinSession(0, NAME_GameSession, *Found);
    }
}

void UPolyPalsGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
    if (SessionInterface.IsValid())
    {
        SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteHandle);
    }

    TArray<FLobbyInfo> Results;
    if (bWasSuccessful && SessionSearch.IsValid())
    {
        for (const FOnlineSessionSearchResult& Result : SessionSearch->SearchResults)
        {
            FLobbyInfo Info;
            Info.SessionId = Result.GetSessionIdStr();
            Info.MaxPlayers = Result.Session.SessionSettings.NumPublicConnections;
            Info.CurrentPlayers = Info.MaxPlayers - Result.Session.NumOpenPublicConnections;
            FString CustomName;
            if (!Result.Session.SessionSettings.Get(TEXT("LobbyName"), CustomName))
            {
                CustomName = Result.Session.OwningUserName;
            }
            Info.LobbyName = CustomName;
            Result.Session.SessionSettings.Get(TEXT("InProgress"), Info.bInProgress);
            Results.Add(Info);
        }
    }

    OnSessionsFound.Broadcast(Results);
}

void UPolyPalsGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    if (SessionInterface.IsValid())
    {
        SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteHandle);

        if (Result == EOnJoinSessionCompleteResult::Success)
        {
            FString ConnectString;
            if (SessionInterface->GetResolvedConnectString(SessionName, ConnectString))
            {
                if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
                {
                    PC->ClientTravel(ConnectString, ETravelType::TRAVEL_Absolute);
                }
            }
        }
    }
}