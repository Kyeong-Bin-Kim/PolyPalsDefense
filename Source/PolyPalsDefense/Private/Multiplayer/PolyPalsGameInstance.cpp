#include "PolyPalsGameInstance.h"
#include "PolyPalsController.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "Online/OnlineSessionNames.h"
#include "Kismet/GameplayStatics.h"

void UPolyPalsGameInstance::Init()
{
    Super::Init();

    // 1) World 컨텍스트 기반 OnlineSubsystem 가져오기
    IOnlineSubsystem* OSS = Online::GetSubsystem(GetWorld());
    if (OSS)
    {
        IdentityInterface = OSS->GetIdentityInterface();
        SessionInterface = OSS->GetSessionInterface();
        UE_LOG(LogTemp, Log, TEXT("Init(): Using OSS from world, Interface 초기화 완료"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Init(): World OSS를 찾을 수 없습니다."));
    }

    // 2) 델리게이트 바인딩
    if (IdentityInterface.IsValid())
    {
        OnLoginCompleteHandle = IdentityInterface->AddOnLoginCompleteDelegate_Handle(
            0,
            FOnLoginCompleteDelegate::CreateUObject(this, &UPolyPalsGameInstance::OnLoginComplete)
        );
    }
    if (SessionInterface.IsValid())
    {
        OnCreateSessionCompleteHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(
            FOnCreateSessionCompleteDelegate::CreateUObject(this, &UPolyPalsGameInstance::OnCreateSessionComplete)
        );
    }

    // 3) Steam 로그인 시도
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

    if (OnCreateSessionCompleteHandle.IsValid())
    {
        SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteHandle);
    }

    // 2) OnCreateSessionComplete 바인딩 (콜백에서 StartSession 등 후속 처리)
    SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(
        FOnCreateSessionCompleteDelegate::CreateUObject(this, &UPolyPalsGameInstance::OnCreateSessionComplete)
    );

    // 3) 세션 세팅
    FOnlineSessionSettings Settings;
    Settings.bIsLANMatch                    = false;
    Settings.NumPublicConnections           = 4;
    Settings.bShouldAdvertise               = true;
    Settings.bUseLobbiesIfAvailable         = true;     // Steam 로비 API 사용
    Settings.bAllowJoinViaPresence          = true;     // 프레젠스 조인 허용
    Settings.bUsesPresence                  = true;     // 프레젠스 자체 사용 플래그
    Settings.bAllowInvites                  = true;     // 친구 초대 허용

    // 4) 호스트 닉네임 가져오기
    FString PlayerName = TEXT("Host");

    if (auto UserId = IdentityInterface->GetUniquePlayerId(0); UserId.IsValid())
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
        0,
        EOnlineDataAdvertisementType::ViaOnlineService
    );

    UE_LOG(LogTemp, Log, TEXT("Flags @Create — LobbiesIfAvailable=%d, UsesPresence=%d"),
        Settings.bUseLobbiesIfAvailable, Settings.bUsesPresence);

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
    }
}

void UPolyPalsGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    if (SessionInterface.IsValid())
    {
        SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteHandle);
    }

    UE_LOG(LogTemp, Log, TEXT("Session creation %s"), bWasSuccessful ? TEXT("succeeded") : TEXT("failed"));

    if (bWasSuccessful && SessionInterface.IsValid())
    {
        // StartSession을 호출
        SessionInterface->StartSession(SessionName);
        UE_LOG(LogTemp, Log, TEXT("StartSession called for %s"), *SessionName.ToString());
    }
}

void UPolyPalsGameInstance::FindSteamSessions()
{
    if (!SessionInterface.IsValid())
        return;

    // 검색 객체 생성 및 설정
    SessionSearch = MakeShared<FOnlineSessionSearch>();
    SessionSearch->bIsLanQuery = false;
    SessionSearch->MaxSearchResults = 100;
    SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

    // 콜백 바인딩
    OnFindSessionsCompleteHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(
        FOnFindSessionsCompleteDelegate::CreateUObject(this, &UPolyPalsGameInstance::OnFindSessionsComplete)
    );

    SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UPolyPalsGameInstance::JoinSteamSession(const FString& LobbyID)
{
    if (!SessionInterface.IsValid() || !SessionSearch.IsValid())
        return;

    if (SessionInterface->GetNamedSession(NAME_GameSession) != nullptr)
    {
        PendingJoinSessionId = LobbyID;
        OnDestroySessionCompleteHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(
            FOnDestroySessionCompleteDelegate::CreateUObject(this, &UPolyPalsGameInstance::OnDestroySessionForJoinComplete));
        SessionInterface->DestroySession(NAME_GameSession);
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
        for (FOnlineSessionSearchResult& PatchResult : SessionSearch->SearchResults)
        {
            bool bPresence = PatchResult.Session.SessionSettings.bUsesPresence;
            PatchResult.Session.SessionSettings.bUseLobbiesIfAvailable = bPresence;

            UE_LOG(LogTemp, Log, TEXT("[FindResult] Patched Flags — LobbiesIfAvailable=%d, UsesPresence=%d"),
                PatchResult.Session.SessionSettings.bUseLobbiesIfAvailable,
                PatchResult.Session.SessionSettings.bUsesPresence);
        }

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
        else  
        {  
            FString ResultString;  
            switch (Result)  
            {  
                case EOnJoinSessionCompleteResult::Success:  
                    ResultString = TEXT("Success");  
                    break;  
                case EOnJoinSessionCompleteResult::SessionIsFull:  
                    ResultString = TEXT("Session is full");  
                    break;  
                case EOnJoinSessionCompleteResult::SessionDoesNotExist:  
                    ResultString = TEXT("Session does not exist");  
                    break;  
                case EOnJoinSessionCompleteResult::CouldNotRetrieveAddress:  
                    ResultString = TEXT("Could not retrieve address");  
                    break;  
                case EOnJoinSessionCompleteResult::AlreadyInSession:  
                    ResultString = TEXT("Already in session");  
                    break;  
                case EOnJoinSessionCompleteResult::UnknownError:  
                    ResultString = TEXT("Unknown error");  
                    break;  
                default:  
                    ResultString = TEXT("Invalid result");  
                    break;  
            }  

            UE_LOG(LogTemp, Warning, TEXT("Failed to join session: %s"), *ResultString);  
        }  
    }  
}

void UPolyPalsGameInstance::PerformJoinSession(const FString& LobbyID)
{
    if (!SessionInterface.IsValid() || !SessionSearch.IsValid())
        return;

    // 1) 검색 결과에서 일치하는 세션 찾기
    const FOnlineSessionSearchResult* Found = SessionSearch->SearchResults.FindByPredicate(
        [&](const FOnlineSessionSearchResult& Result)
        {
            return Result.GetSessionIdStr() == LobbyID;
        });

    if (Found)
    {
        // 2) 복사본 생성 및 플래그 패치
        FOnlineSessionSearchResult SearchResult = *Found;
        SearchResult.Session.SessionSettings.bUseLobbiesIfAvailable =
            SearchResult.Session.SessionSettings.bUsesPresence;

        // 3) 패치된 플래그 로그
        UE_LOG(LogTemp, Log, TEXT("[Join] Patched Flags — LobbiesIfAvailable=%d, UsesPresence=%d"),
            SearchResult.Session.SessionSettings.bUseLobbiesIfAvailable,
            SearchResult.Session.SessionSettings.bUsesPresence);

        // 4) JoinSession 콜백 바인딩
        OnJoinSessionCompleteHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(
            FOnJoinSessionCompleteDelegate::CreateUObject(this, &UPolyPalsGameInstance::OnJoinSessionComplete)
        );

        // 5) 패치된 복사본으로 Join 호출
        SessionInterface->JoinSession(0, NAME_GameSession, SearchResult);
    }
}


void UPolyPalsGameInstance::OnDestroySessionForJoinComplete(FName SessionName, bool bWasSuccessful)
{
    if (SessionInterface.IsValid())
    {
        SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteHandle);
    }

    if (!PendingJoinSessionId.IsEmpty())
    {
        FString Target = PendingJoinSessionId;
        PendingJoinSessionId.Empty();
        PerformJoinSession(Target);
    }
}