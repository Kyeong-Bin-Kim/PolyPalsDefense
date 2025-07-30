#include "Core/Subsystems/TowerDataManager.h"
#include "AssetManagement/PolyPalsDefenseAssetManager.h"
#include "DataAsset/Tower/TowerMaterialData.h"
#include "DataAsset/Tower/TowerPropertyData.h"

bool UTowerDataManager::ShouldCreateSubsystem(UObject* Outer) const
{
	if (const UWorld* World = Cast<UWorld>(Outer))
	{
		if (!World->IsGameWorld())
		{
			return false;
		}

		// EmptyLevel ?紐? 野꺜??
		FString ShortMapName = FPackageName::GetShortName(World->GetMapName());

		if (ShortMapName.Equals(TEXT("EmptyLevel"), ESearchCase::IgnoreCase))
		{
			// ????덇볼???わ쭖???밴쉐??? ??놁벉
			return false;
		}

		return true;
	}

	return false;
}

void UTowerDataManager::OnWorldBeginPlay(UWorld& InWorld)
{
	LoadMaterialData();
	LoadPropertyData();
}

UTowerPropertyData* UTowerDataManager::GetTowerPropertyData(const uint8 InTowerId)
{
	if (!GetWorld()) return nullptr;

	if (TowerProperties.Contains(InTowerId))
	{
		UTowerPropertyData** TargetData = TowerProperties.Find(InTowerId);
		return *TargetData;
	}
	else
	{
		UTowerPropertyData** ErrorData = TowerProperties.Find(0);

		return *ErrorData;
	}
}

int32 UTowerDataManager::GetTowerCost(uint8 InTowerId, ELevelValue InLevel)
{
	if (TowerProperties.IsEmpty()) return 0;
	if (!TowerProperties.Contains(InTowerId)) return 0;

	UTowerPropertyData** FoundData = TowerProperties.Find(InTowerId);
	UTowerPropertyData* Data = *FoundData;
	FTowerUpgradeValue* UpgradeData = Data->UpgradeData.Find(InLevel);

	return UpgradeData->Cost;
}

float UTowerDataManager::GetAbilityDuration(uint8 InTowerId, uint8 InLevel)
{
	float Duration = 0.f;
	if (!TowerProperties.Contains(InTowerId)) return Duration;

	UTowerPropertyData** FoundData = TowerProperties.Find(InTowerId);
	UTowerPropertyData* TowerData = *FoundData;

	ELevelValue TowerLevel = static_cast<ELevelValue>(InLevel - 1);
	ETowerAbility TowerAbility = TowerData->TowerAbility;
	switch (TowerAbility)
	{
	case ETowerAbility::Slow:
	{
		FSlowDetail* FoundDetail = TowerData->SlowDetails.Find(TowerLevel);
		Duration = FoundDetail->Duration;
		break;
	}
	case ETowerAbility::Stun:
	{
		FStunDetail* FoundDetail = TowerData->StunDetails.Find(TowerLevel);
		Duration = FoundDetail->Duration;
		break;
	}
	}

	return Duration;
}

float UTowerDataManager::GetAbilityIntensity(uint8 InTowerId, uint8 InLevel)
{
	float Intensity = 0.f;
	if (!TowerProperties.Contains(InTowerId)) return Intensity;

	UTowerPropertyData** FoundData = TowerProperties.Find(InTowerId);
	UTowerPropertyData* TowerData = *FoundData;

	ELevelValue TowerLevel = static_cast<ELevelValue>(InLevel - 1);
	ETowerAbility TowerAbility = TowerData->TowerAbility;
	switch (TowerAbility)
	{
	case ETowerAbility::Slow:
	{
		FSlowDetail* FoundDetail = TowerData->SlowDetails.Find(TowerLevel);
		Intensity = FoundDetail->Intensity;
		break;
	}
	}

	return Intensity;
}

void UTowerDataManager::LoadMaterialData()
{
	UPolyPalsDefenseAssetManager& Manager = UPolyPalsDefenseAssetManager::Get();
	FPrimaryAssetId MaterialAssetId = FPrimaryAssetId(TEXT("TowerMaterials"), TEXT("Data_TowerMaterial"));

	FStreamableDelegate OnLoadedMaterials = FStreamableDelegate::CreateUObject(
		this, &UTowerDataManager::OnTowerMaterialLoaded, MaterialAssetId
	);
	Manager.LoadPrimaryAsset(MaterialAssetId, {}, OnLoadedMaterials);

}

void UTowerDataManager::LoadPropertyData()
{
	UPolyPalsDefenseAssetManager& Manager = UPolyPalsDefenseAssetManager::Get();
	TArray<FPrimaryAssetId> Assets;
	Manager.GetPrimaryAssetIdList(TEXT("TowerData"), Assets);

	for (const auto& AssetId : Assets)
	{
		Manager.LoadPrimaryAsset(
			AssetId,
			TArray<FName>{},
			FStreamableDelegate::CreateUObject(
				this,
				&UTowerDataManager::OnTowerPropertyLoaded,
				AssetId
			)
		);
	}
}

void UTowerDataManager::OnTowerMaterialLoaded(FPrimaryAssetId LoadedId)
{
	UPolyPalsDefenseAssetManager& Manager = UPolyPalsDefenseAssetManager::Get();
	UTowerMaterialData* LoadedAsset = Cast<UTowerMaterialData>(Manager.GetPrimaryAssetObject(LoadedId));

	TowerMaterials = LoadedAsset;
}

void UTowerDataManager::OnTowerPropertyLoaded(FPrimaryAssetId LoadedId)
{
	UPolyPalsDefenseAssetManager& Manager = UPolyPalsDefenseAssetManager::Get();
	UTowerPropertyData* LoadedAsset = Cast<UTowerPropertyData>(Manager.GetPrimaryAssetObject(LoadedId));
	
	TowerProperties.Add(LoadedAsset->TowerId, LoadedAsset);
}