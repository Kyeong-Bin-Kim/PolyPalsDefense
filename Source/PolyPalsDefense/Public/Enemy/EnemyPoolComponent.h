#pragma once  

#include "CoreMinimal.h"  
#include "Components/ActorComponent.h"  
#include "UObject/PrimaryAssetId.h"
#include "EnemyPoolComponent.generated.h"  

class AEnemyPawn;  

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))  
class POLYPALSDEFENSE_API UEnemyPoolComponent : public UActorComponent  
{  
   GENERATED_BODY()  

public:  
   UEnemyPoolComponent();  

   AEnemyPawn* AcquireEnemy(const FPrimaryAssetId& AssetId);  

   void ReleaseEnemy(AEnemyPawn* Enemy);  

protected:  
   virtual void BeginPlay() override;  

private:  
   TMap<FPrimaryAssetId, TArray<AEnemyPawn*>> EnemyPool;  

   UPROPERTY(EditAnywhere)  
   TSubclassOf<AEnemyPawn> EnemyClass;  
};
