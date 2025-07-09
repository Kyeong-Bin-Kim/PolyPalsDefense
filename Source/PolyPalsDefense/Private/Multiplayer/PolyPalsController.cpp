#include "PolyPalsController.h"
#include "PolyPalsGameMode.h"
#include "PolyPalsPlayerState.h"
#include "PolyPalsState.h"
#include "PolyPalsGamePawn.h"
#include "PolyPalsGameInstance.h"
#include "EnhancedInputComponent.h"
#include "PolyPalsController/GamePawnComponent.h"
#include "PolyPalsController/PolyPalsInputComponent.h"
#include "PolyPalsGamePawn/BuildTowerComponent.h"
#include "PolyPalsGamePawn/TowerHandleComponent.h"
#include "MainUIWidget.h"
#include "StageSelectUIWidget.h"
#include "LobbyListWidget.h"
#include "LobbyUIWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSessionSettings.h"  
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSubsystemTypes.h" 
#include "Net/UnrealNetwork.h"

APolyPalsController::APolyPalsController()
{
	GamePawnComponent = CreateDefaultSubobject<UGamePawnComponent>(TEXT("GamePawnComponent"));
	GamePawnComponent->SetIsReplicated(true);
}

void APolyPalsController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController())
		return;

	bShowMouseCursor = true;
	SetupInitialUIWidgets();

	const FString MapName = UGameplayStatics::GetCurrentLevelName(this, true);

	if (MapName.Equals(TEXT("EmptyLevel")))
	{
		// 1) 이미 세션에 참가된 상태인지 체크
		bool bInSession = false;

		if (IOnlineSubsystem* OSS = IOnlineSubsystem::Get())
		{
			IOnlineSessionPtr Sess = OSS->GetSessionInterface();

			if (Sess.IsValid())
			{
				bInSession = (Sess->GetNamedSession(NAME_GameSession) != nullptr);
			}
		}

		if (bInSession)
		{
			ShowLobbyUI();   // 네트워크 연결 직후
		}
		else
		{
			ShowMainUI();    // 최초 실행 시 메인 메뉴
		}
	}
	else
	{
		HideAllUI();
	}
}

void APolyPalsController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(APolyPalsController, PlayerColor);
}

void APolyPalsController::InitializeAndShowLobbyUI(FName InStageName, const FString& HostName)
{
	ShowLobbyUI();								// UI 보이기 + RefreshLobbyUI
	ConfigureLobbyUI(InStageName, HostName);	// 방 제목, 스테이지 세팅
	RefreshLobbyUI();							// 숫자·슬롯 재갱신
}

void APolyPalsController::Server_SetSelectedStage_Implementation(FName StageName)
{
	if (APolyPalsState* GS = GetWorld()->GetGameState<APolyPalsState>())
	{
		GS->SetSelectedStage(StageName);
	}
}

void APolyPalsController::Server_CreateLobby_Implementation(FName StageName, const FString& HostName)
{
	if (APolyPalsGameMode* GM = GetWorld()->GetAuthGameMode<APolyPalsGameMode>())
	{
		GM->ConfigureLobby(StageName, HostName);
	}
}

void APolyPalsController::Client_ShowLobbyUI_Implementation(const FString& InHostName, FName InStageName, const FString& InLobbyName)
{
	if (!IsLocalController())
	{
		return;
	}

	InitializeAndShowLobbyUI(InStageName, InHostName);
}

void APolyPalsController::Server_SetReady_Implementation(bool bReady)
{
	APolyPalsPlayerState* MyState = GetPlayerState<APolyPalsPlayerState>();
	if (MyState)
	{
		MyState->SetReadyState(bReady);

		APolyPalsState* GameState = GetWorld()->GetGameState<APolyPalsState>();
		if (GameState)
		{
			GameState->UpdateReadyPlayers();
		}
	}
}

void APolyPalsController::SetPlayerColor(EPlayerColor InColor) {
	uint8 EnumToInt = static_cast<uint8>(InColor);
	UE_LOG(LogTemp, Log, TEXT("APolyPalsController: Player color set to: %d"), EnumToInt);
	PlayerColor = InColor; 

}

void APolyPalsController::SetLobbyUIInstance(ULobbyUIWidget* InWidget)
{
	LobbyUIInstance = InWidget;
}

void APolyPalsController::UpdatePlayerNickname(const FString& NewName)
{
	if (APolyPalsPlayerState* PS = GetPlayerState<APolyPalsPlayerState>())
	{
		PS->SetPlayerName(NewName);
	}

	if (UMainUIWidget* Widget = Cast<UMainUIWidget>(MainUIWidgetInstance))
	{
		Widget->SetPlayerNameText(NewName);
	}
}

void APolyPalsController::UpdateReadyUI(APolyPalsPlayerState* ChangedPlayerState, bool bIsReady)
{
	if (LobbyUIInstance)
	{
		LobbyUIInstance->UpdatePlayerSlotReadyState(ChangedPlayerState, bIsReady);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("LobbyUIInstance가 없습니다. Ready 상태를 UI에 반영할 수 없습니다."));
	}
}

void APolyPalsController::RefreshLobbyUI()
{
	if (!IsLocalController() || !LobbyUIInstance)
	{
		return;
	}

	if (APolyPalsState* GS = GetWorld()->GetGameState<APolyPalsState>())
	{
		LobbyUIInstance->SetRoomTitle(GS->GetLobbyName());
		LobbyUIInstance->SetSelectedStage(GS->GetSelectedStage());

		// 방 제목, 스테이지도 갱신
		LobbyUIInstance->SetRoomTitle(GS->GetLobbyName());
		LobbyUIInstance->SetSelectedStage(GS->GetSelectedStage());

		// 기존 로직: 접속/준비 수, 슬롯 갱신
		LobbyUIInstance->UpdateLobbyInfo(GS->GetConnectedPlayers(), GS->GetReadyPlayers(), GS->GetSelectedStage(), HasAuthority());

		LobbyUIInstance->RefreshPlayerSlots(GS->PlayerArray);
	}
}

void APolyPalsController::HideAllUI()
{
	if (MainUIWidgetInstance) MainUIWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
	if (StageSelectWidgetInstance) StageSelectWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
	if (LobbyListWidgetInstance) LobbyListWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
	if (LobbyUIInstance) LobbyUIInstance->SetVisibility(ESlateVisibility::Hidden);
}

void APolyPalsController::SetupInitialUIWidgets()
{
	if (!MainUIWidgetInstance && MainUIWidgetClass)
	{
		MainUIWidgetInstance = CreateWidget<UMainUIWidget>(this, MainUIWidgetClass);
		if (MainUIWidgetInstance)
		{
			MainUIWidgetInstance->AddToViewport();
			MainUIWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (!StageSelectWidgetInstance && StageSelectWidgetClass)
	{
		StageSelectWidgetInstance = CreateWidget<UStageSelectUIWidget>(this, StageSelectWidgetClass);
		if (StageSelectWidgetInstance)
		{
			StageSelectWidgetInstance->AddToViewport();
			StageSelectWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (!LobbyListWidgetInstance && LobbyListWidgetClass)
	{
		LobbyListWidgetInstance = CreateWidget<ULobbyListWidget>(this, LobbyListWidgetClass);
		if (LobbyListWidgetInstance)
		{
			LobbyListWidgetInstance->AddToViewport();
			LobbyListWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (!LobbyUIInstance && LobbyUIWidgetClass)
	{
		LobbyUIInstance = CreateWidget<ULobbyUIWidget>(this, LobbyUIWidgetClass);
		if (LobbyUIInstance)
		{
			LobbyUIInstance->AddToViewport();
			LobbyUIInstance->SetVisibility(ESlateVisibility::Hidden);
			SetLobbyUIInstance(LobbyUIInstance);
		}
	}
}

void APolyPalsController::ShowMainUI()
{
	SetupInitialUIWidgets();
	HideAllUI();

	if (UMainUIWidget* TypedWidget = Cast<UMainUIWidget>(MainUIWidgetInstance))
	{
		FString PlayerName = TEXT("Player");

		if (IOnlineSubsystem* OSS = IOnlineSubsystem::Get())
		{
			IOnlineIdentityPtr Identity = OSS->GetIdentityInterface();

			if (Identity.IsValid())
			{
				TSharedPtr<const FUniqueNetId> UserId = Identity->GetUniquePlayerId(0);

				if (UserId.IsValid())
				{
					PlayerName = Identity->GetPlayerNickname(*UserId);
				}
				else if (UPolyPalsGameInstance* GI = Cast<UPolyPalsGameInstance>(GetGameInstance()))
				{
					GI->LoginToSteam();
				}
			}
		}
		TypedWidget->SetPlayerNameText(PlayerName);
	}

	if (MainUIWidgetInstance)
	{
		MainUIWidgetInstance->SetVisibility(ESlateVisibility::Visible);
	}
}

void APolyPalsController::ShowStageSelectUI()
{
	SetupInitialUIWidgets();
	HideAllUI();
	if (StageSelectWidgetInstance)
	{
		StageSelectWidgetInstance->SetVisibility(ESlateVisibility::Visible);
	}
}

void APolyPalsController::ShowLobbyListUI()
{
	SetupInitialUIWidgets();
	HideAllUI();
	if (LobbyListWidgetInstance)
	{
		LobbyListWidgetInstance->SetVisibility(ESlateVisibility::Visible);
	}
}

void APolyPalsController::HostLobby(FName StageName, const FString& PlayerName)
{
	Server_CreateLobby(StageName, PlayerName);

	if (auto GI = Cast<UPolyPalsGameInstance>(GetGameInstance()))
	{
		GI->SetPendingLobbyName(PlayerName);
		InitializeAndShowLobbyUI(StageName, PlayerName);
		GI->CreateSteamSession(StageName);
	}
}

void APolyPalsController::ShowLobbyUI()
{
	SetupInitialUIWidgets();
	HideAllUI();

	if (LobbyUIInstance)
	{
		LobbyUIInstance->SetVisibility(ESlateVisibility::Visible);
		RefreshLobbyUI();
	}
}

void APolyPalsController::ConfigureLobbyUI(FName InStageName, const FString& HostName)
{
	if (!LobbyUIInstance)
		return;

	LobbyUIInstance->SetSelectedStage(InStageName);
	LobbyUIInstance->SetRoomTitle(HostName);
}

void APolyPalsController::LeaveLobby()
{
	if (auto GI = Cast<UPolyPalsGameInstance>(GetGameInstance()))
	{
		GI->LeaveSteamSession();
	}

	ShowMainUI();
}

void APolyPalsController::BeginSelectTower(int32 TowerIndex)
{
	PendingTowerIndex = TowerIndex;

	if (APolyPalsGamePawn* MyPawn = Cast<APolyPalsGamePawn>(GetPawn()))
	{
		if (UBuildTowerComponent* BuildComp = MyPawn->GetBuildTowerComponent())
		{
			UE_LOG(LogTemp, Log, TEXT("PolyPalsController BeginSelectTower TowerIndex = %d"), TowerIndex);

			BuildComp->ClientBeginSelectTower(static_cast<uint8>(TowerIndex));
		}
	}
}

void APolyPalsController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!IsLocalController()) return;

	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	UE_LOG(LogTemp, Warning, TEXT(">>> OnPossess called, has UPolyPalsInputComponent=%s"), FindComponentByClass<UPolyPalsInputComponent>() ? TEXT("YES") : TEXT("NO"));

	// EnhancedInput 세팅
	if (UPolyPalsInputComponent* InputComp = FindComponentByClass<UPolyPalsInputComponent>())
	{
		// 바인딩 중복 방지
		// 싱글캐스트 델리게이트는 Unbind()
		InputComp->OnInputTower1.Unbind();
		InputComp->OnInputTower2.Unbind();
		InputComp->OnInputTower3.Unbind();
		InputComp->OnInputRightClick.Unbind();

		// 멀티캐스트 델리게이트는 Clear()
		InputComp->OnInputLeftClick.Clear();

		// 향상된 입력 셋팅
		InputComp->SetupEnhancedInput(this);

		// Pawn, BuildComp, HandleComp 캐스트
		if (APolyPalsGamePawn* MyPawn = Cast<APolyPalsGamePawn>(InPawn))
		{
			UBuildTowerComponent* BuildComp = MyPawn->GetBuildTowerComponent();
			UTowerHandleComponent* HandleComp = MyPawn->GetTowerHandleComponent();

			// 1,2,3 키 → 프리뷰 모드
			InputComp->OnInputTower1.BindUObject(BuildComp, &UBuildTowerComponent::ClientOnInputTower1);
			InputComp->OnInputTower2.BindUObject(BuildComp, &UBuildTowerComponent::ClientOnInputTower2);
			InputComp->OnInputTower3.BindUObject(BuildComp, &UBuildTowerComponent::ClientOnInputTower3);
			
			// 우클릭 → 프리뷰 취소
			InputComp->OnInputRightClick.BindUObject(BuildComp, &UBuildTowerComponent::ClientOnInputRightClick);
			
			// 좌클릭
			//   - 빌드 모드일 땐 설치 확정
			InputComp->OnInputLeftClick.AddUObject(BuildComp, &UBuildTowerComponent::ClientOnInputLeftClick);
			//   - 빌드 모드 아닐 땐 기존 타워 클릭 처리
			InputComp->OnInputLeftClick.AddUObject(HandleComp, &UTowerHandleComponent::HandleLeftClick);
		}
	}

	// 커서 보이게
	bShowMouseCursor = true;

	// UI와 게임 모두 클릭을 받도록 모드 설정
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
}

bool APolyPalsController::Server_BuildTower_Validate(int32 TowerIndex, FVector_NetQuantize InSpawnLocation)
{
	// TowerIndex  체크
	return TowerIndex >= 1 && TowerIndex <= 3;
}

void APolyPalsController::Server_BuildTower_Implementation(int32 TowerIndex, FVector_NetQuantize InSpawnLocation)
{
	// 내 Pawn 과 BuildComp 얻기
	APolyPalsGamePawn * MyPawn = Cast<APolyPalsGamePawn>(GetPawn());

	if (!MyPawn) return;
	
	UBuildTowerComponent * BuildComp = MyPawn->GetBuildTowerComponent();

	if (BuildComp)
	{
		BuildComp->Server_RequestSpawnTower(SpawnLocation, TowerIndex);
	}
}

void APolyPalsController::InitializeControllerDataByGameMode(EPlayerColor InColor)
{
	PlayerColor = InColor;

	if (GamePawnComponent->GetGamePawn())
	{
		UBuildTowerComponent* BuildTowerComp = GamePawnComponent->GetGamePawn()->GetBuildTowerComponent();
		BuildTowerComp->SetPlayerColor(PlayerColor);
	}
}