// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PreviewBuilding.generated.h"

class APolyPalsController;
class UTowerDataManager;
UCLASS()
class POLYPALSDEFENSE_API APreviewBuilding : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APreviewBuilding();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void ExteranlInitialize(APolyPalsController* const InController);

	void ShowPreviewBuilding(bool bShow, uint8 InTowerId = 0);
	void ChangeMeshMaterial(bool bIsBuildable);

	UStaticMeshComponent* GetMeshComponent() const { return MeshComponent; }
	bool IsBuildable() const { return bIsBuildable; }
private:
	FVector GetSnappedLocation(const FVector& WorldLocation);
	void UpdateLocationUnderCursor();

	UFUNCTION()
	void OnPlacedTowerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
private:
	UPROPERTY()
	TObjectPtr<class USceneComponent> RootScene;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	UPROPERTY()
	TObjectPtr<UTowerDataManager> TowerDataManager;
	UPROPERTY()
	TObjectPtr<APolyPalsController> PolyPalsController;
	UPROPERTY()
	TObjectPtr<UMaterialInterface> BuildableMat;
	UPROPERTY()
	TObjectPtr<UMaterialInterface> UnbuildableMat;

	FVector LastSnappedLocation;
	FVector OffsetLocation;
	bool bIsBuildable = true;

private:
	FTimerHandle LineTraceHandle;
};
//https://chatgpt.com/share/6825a9e6-8228-8003-b472-b11a7ad97975