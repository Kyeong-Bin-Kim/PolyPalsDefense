#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "SerializedSpottedEnemy.generated.h"

USTRUCT(BlueprintType)
struct FSpottedEnemy : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY()
	AActor* Enemy;

	bool operator==(const FSpottedEnemy& Other) const
	{
		return Enemy == Other.Enemy;
	}
};

USTRUCT(BlueprintType)
struct FSpottedEnemies : public FFastArraySerializer
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FSpottedEnemy> Enemies;

	void AddEnemy(const FSpottedEnemy& NewEnemy)
	{
		Enemies.AddUnique(NewEnemy);
		MarkItemDirty(Enemies.Last());
	}

	void RemoveEnemy(const FSpottedEnemy& TargetEnemy)
	{
		Enemies.Remove(TargetEnemy);
		MarkArrayDirty();
	}

	// 서버에서 변경 사항을 클라이언트에 동기화할 때 호출되는 함수
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FSpottedEnemy, FSpottedEnemies>(Enemies, DeltaParms, *this);
	}

};

// 네트워크 복제 활성화 매크로
template<>
struct TStructOpsTypeTraits<FSpottedEnemies> : public TStructOpsTypeTraitsBase2<FSpottedEnemies>
{
	enum { WithNetDeltaSerializer = true };
};