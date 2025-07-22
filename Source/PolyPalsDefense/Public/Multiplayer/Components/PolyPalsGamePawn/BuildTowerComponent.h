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
	// Preview 紐⑤뱶 吏꾩엯
	UFUNCTION(Client, Unreliable)
	void ClientBeginSelectTower(uint8 InTowerId);

	// Preview ?꾩튂瑜??쎌뼱?ㅻ뒗 Getter
	UFUNCTION(BlueprintCallable, Category = "Build")
	FVector GetPreviewLocation() const;

	// 理쒖쥌 ?ㅼ튂 ?붿껌 RPC
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestSpawnTower(FVector_NetQuantize InLocation, uint8 InTargetTower);

	void SetPlayerColor(EPlayerColor InColor) { PlayerColor = InColor; }
	EBuildState GetBuildState() const { return BuildState; }
	void SetBuildState(EBuildState InState);

	void OnInputLeftClick();

	void OnInputRightClick();

	void OnInputTower1();

	void OnInputTower2();

	void OnInputTower3();

	// PreviewBuilding ?ㅽ룿
	void SpawnPreviewBuilding();

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
