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

   // ?먯뀑 ID????묓븯???곸쓣 ??먯꽌 爰쇰궡嫄곕굹 ?덈줈 ?앹꽦
   AEnemyPawn* AcquireEnemy(const FPrimaryAssetId& AssetId, USplineComponent* InSpline, float HealthMultiplier, float SpeedMultiplier, bool bIsBoss, FVector Scale);

   // ?ъ슜???앸궃 ?곸쓣 ?濡?諛섑솚
   void ReleaseEnemy(AEnemyPawn* Enemy);  

protected:  
   virtual void BeginPlay() override;

private:
	// ?먯뀑蹂꾨줈 蹂닿??섎뒗 ???
   TMap<FPrimaryAssetId, TArray<AEnemyPawn*>> EnemyPool;  

   // ?덈줈 ?앹꽦?????대옒??
   UPROPERTY(EditAnywhere)  
   TSubclassOf<AEnemyPawn> EnemyClass;

   // ????놁쓣 ???덈줈 ???몄뒪?댁뒪 ?앹꽦
   AEnemyPawn* CreateNewEnemy(const FPrimaryAssetId& AssetId, USplineComponent* InSpline, float HealthMultiplier, float SpeedMultiplier, bool bIsBoss, FVector Scale);
};
