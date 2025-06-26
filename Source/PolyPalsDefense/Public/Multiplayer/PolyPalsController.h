// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Tower/TowerEnums.h"
#include "PolyPalsController.generated.h"

class UPolyPalsInputComponent;
class UGamePawnComponent;
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
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	void Server_SetSelectedStage(FName StageName);

	UFUNCTION(Client, Reliable)
	void Client_ShowLobbyUI(const FString& HostName);

	UFUNCTION(Server, Reliable)
	void Server_SetReady(bool bReady);

	UPolyPalsInputComponent* GetPolyPalsInputComponent() const { return PolyPalsInputComponent; }
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
	UPROPERTY()
	TObjectPtr<UPolyPalsInputComponent> PolyPalsInputComponent;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UGamePawnComponent> GamePawnComponent;

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
