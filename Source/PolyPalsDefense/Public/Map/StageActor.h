#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StageActor.generated.h"

class USplineComponent;
class UDataTable;
class AWaveSpawner;

UCLASS()
class POLYPALSDEFENSE_API AStageActor : public AActor
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

public:
    AStageActor();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Path")
    USplineComponent* PathSpline;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
    UDataTable* WavePlanTable;

    UPROPERTY(VisibleAnywhere, Category = "Wave")
    TArray<AWaveSpawner*> WaveSpawners;

    UPROPERTY(VisibleAnywhere, Category = "Zones")
    USceneComponent* TowerBuildZoneRoot;

    UPROPERTY(VisibleAnywhere, Category = "Zones")
    USceneComponent* PathBlockZoneRoot;

    USplineComponent* GetSpline() const { return PathSpline; }
    UDataTable* GetWavePlanTable() const { return WavePlanTable; }
    void RegisterSpawner(AWaveSpawner* Spawner);
};
