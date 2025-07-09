#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Tower/TowerEnums.h"
#include "PolyPalsController.generated.h"

class UGamePawnComponent;
class UPolyPalsInputComponent;
class UMainUIWidget;
class UStageSelectUIWidget;
class ULobbyListWidget;
class ULobbyUIWidget;

UCLASS()
class POLYPALSDEFENSE_API APolyPalsController : public APlayerController
{
	GENERATED_BODY()
	

public:
	APolyPalsController();

protected:
	virtual void BeginPlay() override;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void InitializeAndShowLobbyUI(FName InStageName, const FString& HostName);

	UFUNCTION(Server, Reliable)
	void Server_SetSelectedStage(FName StageName);

	UFUNCTION(Server, Reliable)
	void Server_CreateLobby(FName StageName, const FString& HostName);

	UFUNCTION(Client, Reliable)
	void Client_ShowLobbyUI(const FString& InHostName, FName InStageName, const FString& InLobbyName);

	UFUNCTION(Server, Reliable)
	void Server_SetReady(bool bReady);

	UFUNCTION(BlueprintCallable)
	void HostLobby(FName StageName, const FString& PlayerName);

	UFUNCTION(BlueprintCallable)
	void LeaveLobby();

	UFUNCTION(BlueprintCallable, Category = "Build")
	void BeginSelectTower(int32 TowerIndex);

	// UI 에서 눌러서 타워를 지을 때 서버로 전달할 RPC
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_BuildTower(int32 TowerIndex, FVector_NetQuantize InSpawnLocation);

	UGamePawnComponent* GetGamePawnComponent() const { return GamePawnComponent; }
	EPlayerColor GetPlayerColor() const { return PlayerColor; }

	void SetPlayerColor(EPlayerColor InColor);

	void SetLobbyUIInstance(class ULobbyUIWidget* InWidget);
	void UpdatePlayerNickname(const FString& NewName);
	void UpdateReadyUI(class APolyPalsPlayerState* ChangedPlayerState, bool bIsReady);
	void RefreshLobbyUI();
	void InitializeControllerDataByGameMode(EPlayerColor InColor);

	void ShowMainUI();
	void ShowStageSelectUI();
	void ShowLobbyListUI();
	void ShowLobbyUI();
	void ConfigureLobbyUI(FName InStageName, const FString& HostName);

protected:
	virtual void OnPossess(APawn* InPawn) override;

private:
	// 마우스 클릭 확정 시 이 ID로 빌드 RPC 호출
	int32 PendingTowerIndex = 0;

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UGamePawnComponent> GamePawnComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
	UPolyPalsInputComponent* PolyPalsInputComponent;

	// UI 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UMainUIWidget> MainUIWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UStageSelectUIWidget> StageSelectWidgetClass;

	UPROPERTY()
	TObjectPtr<UStageSelectUIWidget> StageSelectWidgetInstance;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class ULobbyListWidget> LobbyListWidgetClass;

	UPROPERTY()
	TObjectPtr<ULobbyListWidget> LobbyListWidgetInstance;

	// UI 인스턴스
	UPROPERTY()
	TObjectPtr<UUserWidget> MainUIWidgetInstance;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class ULobbyUIWidget> LobbyUIWidgetClass;

	UPROPERTY()
	TObjectPtr<class ULobbyUIWidget> LobbyUIInstance;

	void HideAllUI();
	void SetupInitialUIWidgets();

	UPROPERTY(Replicated)
	EPlayerColor PlayerColor = EPlayerColor::None;

	friend class APolyPalsGameMode;
};
