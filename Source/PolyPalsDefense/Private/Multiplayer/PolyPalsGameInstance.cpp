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

void UPolyPalsGameInstance::CreateSteamSession(FName InStageName)
{
    PendingStageName = InStageName;

    if (!SessionInterface.IsValid() || !IdentityInterface.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("세션 인터페이스나 아이덴티티 인터페이스가 유효하지 않습니다."));
        return;
    }

    if (SessionInterface->GetNamedSession(NAME_GameSession) != nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("기존 세션이 있어 DestroySession 후 진행"));

        OnDestroySessionForCreateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(
            FOnDestroySessionCompleteDelegate::CreateUObject(this, &UPolyPalsGameInstance::OnDestroySessionForCreateComplete)
        );

        SessionInterface->DestroySession(NAME_GameSession);
        return;
    }

    CreateSteamSession_Internal(InStageName);
}

void UPolyPalsGameInstance::OnDestroySessionForCreateComplete(FName SessionName, bool bWasSuccessful)
{
    UE_LOG(LogTemp, Log, TEXT("세션 파괴 완료: %s, 성공 여부: %d"), *SessionName.ToString(), bWasSuccessful);

    if (SessionInterface.IsValid())
    {
        SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionForCreateHandle);
    }

    // 파괴 성공 시 내부 세션 생성 호출
    if (bWasSuccessful)
    {
        CreateSteamSession_Internal(PendingStageName);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("세션 삭제 실패, 새 세션 생성 중단"));
    }
}

void UPolyPalsGameInstance::CreateSteamSession_Internal(FName InStageName)
{
    UE_LOG(LogTemp, Log, TEXT("CreateSteamSession_Internal called"));

    FOnlineSessionSettings Settings;
    Settings.bIsLANMatch = false;
    Settings.NumPublicConnections = MaxPlayerCount;
    Settings.bShouldAdvertise = true;
    Settings.bUseLobbiesIfAvailable = true;
    Settings.bAllowJoinViaPresence = true;
    Settings.bAllowJoinInProgress = true;
    Settings.bUsesPresence = true;
    Settings.bAllowInvites = true;

    Settings.Set(FName(TEXT("SEARCH_KEYWORDS")), FString(TEXT("PolyPalsDefense")),
        EOnlineDataAdvertisementType::ViaOnlineService);

    // 닉네임 기반 로비 이름 설정
    FString PlayerName = TEXT("Host");
    if (auto UserId = IdentityInterface->GetUniquePlayerId(0); UserId.IsValid())
    {
        PlayerName = IdentityInterface->GetPlayerNickname(*UserId);
    }

    FString LobbyName = FString::Printf(TEXT("%s's Room"), *PlayerName);
    Settings.Set(TEXT("LobbyName"), LobbyName, EOnlineDataAdvertisementType::ViaOnlineService);
    Settings.Set(TEXT("InProgress"), 0, EOnlineDataAdvertisementType::ViaOnlineService);

    FString StageNameString = InStageName.ToString();
    Settings.Set(TEXT("StageName"), StageNameString, EOnlineDataAdvertisementType::ViaOnlineService);

    // 델리게이트 재설정
    OnCreateSessionCompleteHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(
        FOnCreateSessionCompleteDelegate::CreateUObject(this, &UPolyPalsGameInstance::OnCreateSessionComplete)
    );

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
    // 1) 델리게이트 해제
    if (SessionInterface.IsValid())
    {
        SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteHandle);
    }

    UE_LOG(LogTemp, Log, TEXT("Session creation %s"), bWasSuccessful ? TEXT("succeeded") : TEXT("failed"));
    if (!bWasSuccessful || !SessionInterface.IsValid())
    {
        return;
    }

    // 2) 세션 시작
    const bool bStartCalled = SessionInterface->StartSession(SessionName);
    UE_LOG(LogTemp, Log, TEXT("StartSession called for %s, Result=%d"), *SessionName.ToString(), bStartCalled);

    // 3) (디버그) 실제 세션 상태 확인
    if (FNamedOnlineSession* Session = SessionInterface->GetNamedSession(SessionName))
    {
        UE_LOG(LogTemp, Log, TEXT("Session state after StartSession: %d"), static_cast<int32>(Session->SessionState));
    }

    // 4) 호스트(서버) 레벨 이동 – listen 모드로 열어줍니다.
    UWorld* World = GetWorld();
    if (World && World->GetNetMode() == NM_Standalone)
    {
        // ?listen 옵션을 붙여야 다른 클라이언트들이 이 서버에 접속할 수 있습니다.
        World->ServerTravel(TEXT("/Game/EmptyLevel?listen"));
    }
}

void UPolyPalsGameInstance::FindSteamSessions()
{
    if (!SessionInterface.IsValid())
        return;

    UE_LOG(LogTemp, Log, TEXT("FindSteamSessions called"));

    // 검색 객체 생성 및 설정
    SessionSearch = MakeShared<FOnlineSessionSearch>();
    SessionSearch->bIsLanQuery = false;
    SessionSearch->MaxSearchResults = 100;
    SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

    SessionSearch->QuerySettings.Set(FName(TEXT("SEARCH_KEYWORDS")), FString(TEXT("PolyPalsDefense")),
        EOnlineComparisonOp::Equals);

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

    UE_LOG(LogTemp, Log, TEXT("JoinSteamSession called with ID %s"), *LobbyID);

    if (SessionInterface->GetNamedSession(NAME_GameSession) != nullptr)
    {
        UE_LOG(LogTemp, Log, TEXT("Existing session found. Destroying current session before join"));

        PendingJoinSessionId = LobbyID;
        OnDestroySessionCompleteHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(
            FOnDestroySessionCompleteDelegate::CreateUObject(this, &UPolyPalsGameInstance::OnDestroySessionForJoinComplete));
        SessionInterface->DestroySession(NAME_GameSession);
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("No existing session. Joining directly"));
        PerformJoinSession(LobbyID);
    }
}

void UPolyPalsGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
    if (SessionInterface.IsValid())
    {
        SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteHandle);
    }

    UE_LOG(LogTemp, Log, TEXT("OnFindSessionsComplete: Success=%d, NumResults=%d"), bWasSuccessful, SessionSearch.IsValid() ? SessionSearch->SearchResults.Num() : 0);

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
            UE_LOG(LogTemp, Log, TEXT("Found session %s"), *Result.GetSessionIdStr());

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
    // 1) 델리게이트 해제
    if (SessionInterface.IsValid())
    {
        SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteHandle);
    }

    UE_LOG(LogTemp, Log, TEXT("OnJoinSessionComplete: %s, Result=%d"), *SessionName.ToString(), (int)Result);
    if (Result != EOnJoinSessionCompleteResult::Success || !SessionInterface.IsValid())
    {
        return;
    }

    // 2) 세션 접속 문자열 얻기
    FString ConnectString;
    if (!SessionInterface->GetResolvedConnectString(SessionName, ConnectString))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to resolve connect string for session %s"), *SessionName.ToString());
        return;
    }
    UE_LOG(LogTemp, Log, TEXT("ResolvedConnectString = %s"), *ConnectString);

    // 3) 맵 경로만 URL에 추가
    FString TravelURL = ConnectString;
    TravelURL += ConnectString.Contains(TEXT("?")) ? TEXT("&") : TEXT("?");
    TravelURL += TEXT("Game=/Game/EmptyLevel");

    // 4) 클라이언트 트래블
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
    {
        UE_LOG(LogTemp, Log, TEXT("ClientTravel to %s"), *TravelURL);
        PC->ClientTravel(*TravelURL, ETravelType::TRAVEL_Absolute);
    }
}

void UPolyPalsGameInstance::LeaveSteamSession()
{
    if (!SessionInterface.IsValid())
        return;

    if (SessionInterface->GetNamedSession(NAME_GameSession) != nullptr)
    {
        OnDestroySessionCompleteHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(
            FOnDestroySessionCompleteDelegate::CreateUObject(this, &UPolyPalsGameInstance::OnLeaveSessionComplete));
        SessionInterface->DestroySession(NAME_GameSession);
    }
}

void UPolyPalsGameInstance::OnLeaveSessionComplete(FName SessionName, bool bWasSuccessful)
{
    if (SessionInterface.IsValid())
    {
        SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteHandle);
    }

    UE_LOG(LogTemp, Log, TEXT("Leave session complete: %s, Success=%d"), *SessionName.ToString(), bWasSuccessful);
}

void UPolyPalsGameInstance::PerformJoinSession(const FString& LobbyID)
{
    if (!SessionInterface.IsValid() || !SessionSearch.IsValid())
        return;

    UE_LOG(LogTemp, Log, TEXT("PerformJoinSession with ID %s"), *LobbyID);

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
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Requested session %s not found in search results"), *LobbyID);
    }
}

void UPolyPalsGameInstance::OnDestroySessionForJoinComplete(FName SessionName, bool bWasSuccessful)
{
    if (SessionInterface.IsValid())
    {
        SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteHandle);
    }

    UE_LOG(LogTemp, Log, TEXT("OnDestroySessionForJoinComplete: %s, Success=%d"), *SessionName.ToString(), bWasSuccessful);

    if (!PendingJoinSessionId.IsEmpty())
    {
        FString Target = PendingJoinSessionId;
        PendingJoinSessionId.Empty();
        PerformJoinSession(Target);
    }
}