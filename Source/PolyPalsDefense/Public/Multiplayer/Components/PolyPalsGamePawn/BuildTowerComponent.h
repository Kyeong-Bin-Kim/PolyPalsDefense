#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataAsset/Tower/TowerEnums.h"
#include "UI/TowerUpgradeWidget.h"
#include "BuildTowerComponent.generated.h"

class UTowerDataManager;
class APreviewBuilding;
class APlacedTower;

DECLARE_DELEGATE(FTryBuildButNotEnoughGold)

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class POLYPALSDEFENSE_API UBuildTowerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBuildTowerComponent();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// Preview 모드 진입
	UFUNCTION(Client, Unreliable)
	void ClientBeginSelectTower(uint8 InTowerId);

	// Preview 위치를 읽어오는 Getter
	UFUNCTION(BlueprintCallable, Category = "Build")
	FVector GetPreviewLocation() const;

	// 최종 설치 요청 RPC
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestSpawnTower(const FVector_NetQuantize InLocation, uint8 InTargetTower);

	// PreviewBuilding 스폰
	void ClientSpawnPreviewBuilding();

	void SetPlayerColor(EPlayerColor InColor) { PlayerColor = InColor; }
	EBuildState GetBuildState() const { return BuildState; }
	void SetBuildState(EBuildState InState);

	UFUNCTION(Client, Unreliable)
	void ClientOnInputLeftClick();

	UFUNCTION(Client, Unreliable)
	void ClientOnInputRightClick();

	UFUNCTION(Client, Unreliable)
	void ClientOnInputTower1();

	UFUNCTION(Client, Unreliable)
	void ClientOnInputTower2();

	UFUNCTION(Client, Unreliable)
	void ClientOnInputTower3();

public:
	FTryBuildButNotEnoughGold TryBuildButNotEnoughGold;

private:
	void OnSelectTower(uint8 InTowerId);
	void OnNormal();
	void OnSerchingPlace();
	void OnDecidePlacementLocation();

private:
	UPROPERTY()
	TObjectPtr<UTowerDataManager> TowerDataManager;

	UPROPERTY()
	TObjectPtr<APreviewBuilding> PreviewBuilding;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TSubclassOf<APlacedTower> PlacedTowerBlueClass;

	UPROPERTY(Replicated)
	EPlayerColor PlayerColor = EPlayerColor::None;

	uint8 TowerOnSearchingQue = 0;

	EBuildState BuildState = EBuildState::None;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UTowerUpgradeWidget> TowerUpgradeWidgetClass;

	UPROPERTY()
	TObjectPtr<UTowerUpgradeWidget> UpgradeWidgetInstance;
};
