// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PreviewBuilding.generated.h"

class APolyPalsController;
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

	void ShowPreviewBuilding(bool bShow);
	
private:
	FVector GetSnappedLocation(const FVector& WorldLocation);
	void UpdateLocationUnderCursor();

private:
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	UPROPERTY()
	TObjectPtr<APolyPalsController> PolyPalsController;

private:
	FTimerHandle LineTraceHandle;
};
