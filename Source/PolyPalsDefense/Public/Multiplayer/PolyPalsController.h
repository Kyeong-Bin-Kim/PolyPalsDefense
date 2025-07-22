#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Tower/TowerEnums.h"
#include "PolyPalsController.generated.h"

class UGamePawnComponent;
class UPolyPalsInputComponent;
class UMainUIWidget;
class UStageSelectUIWidget;
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
	virtual void OnRep_Pawn() override;

	virtual void SetupInputComponent() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	void Server_SetReady(bool bReady);

	UFUNCTION(BlueprintCallable)
	void LeaveLobby();

	UFUNCTION(BlueprintCallable, Category = "Build")
	void BeginSelectTower(int32 TowerIndex);

	// UI ?먯꽌 ?뚮윭????뚮? 吏?????쒕쾭濡??꾨떖??RPC
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
	void ShowLobbyUI();
	void InitializeAndShowLobbyUI(FName InStageName, const FString& HostName);
	void ConfigureLobbyUI(FName InStageName, const FString& HostName);

private:
	// 留덉슦???대┃ ?뺤젙 ????ID濡?鍮뚮뱶 RPC ?몄텧
	int32 PendingTowerIndex = 0;

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UGamePawnComponent> GamePawnComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
	UPolyPalsInputComponent* PolyPalsInputComponent;

	// UI ?꾩젽 ?대옒??
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UMainUIWidget> MainUIWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UStageSelectUIWidget> StageSelectWidgetClass;

	UPROPERTY()
	TObjectPtr<UStageSelectUIWidget> StageSelectWidgetInstance;

	// UI ?몄뒪?댁뒪
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
