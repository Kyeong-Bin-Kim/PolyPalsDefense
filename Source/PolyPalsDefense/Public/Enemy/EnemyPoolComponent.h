#pragma once  

#include "CoreMinimal.h"  
#include "Components/ActorComponent.h"  
#include "UObject/PrimaryAssetId.h"
#include "EnemyPoolComponent.generated.h"  

class AEnemyPawn;
class USplineComponent;
class AStageActor;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))  
class POLYPALSDEFENSE_API UEnemyPoolComponent : public UActorComponent  
{  
   GENERATED_BODY()  

public:  
   UEnemyPoolComponent();  

   AEnemyPawn* AcquireEnemy(const FPrimaryAssetId& AssetId, USplineComponent* InSpline, float HealthMultiplier, float SpeedMultiplier, bool bIsBoss, FVector Scale);

   void ReleaseEnemy(AEnemyPawn* Enemy);  

protected:  
   virtual void BeginPlay() override;

private:
   TMap<FPrimaryAssetId, TArray<AEnemyPawn*>> EnemyPool;  

   UPROPERTY(EditAnywhere)  
   TSubclassOf<AEnemyPawn> EnemyClass;

   AEnemyPawn* CreateNewEnemy(const FPrimaryAssetId& AssetId, USplineComponent* InSpline, float HealthMultiplier, float SpeedMultiplier, bool bIsBoss, FVector Scale);
};
