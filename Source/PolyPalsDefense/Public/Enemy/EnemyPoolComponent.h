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

   // ?癒??ID?????臾볥릭???怨몄뱽 ???癒?퐣 ?곗눖沅▼쳞怨뺢돌 ??덉쨮 ??밴쉐
   AEnemyPawn* AcquireEnemy(const FPrimaryAssetId& AssetId, USplineComponent* InSpline, float HealthMultiplier, float SpeedMultiplier, bool bIsBoss, FVector Scale);

   // ???????멸텆 ?怨몄뱽 ??嚥?獄쏆꼹??
   void ReleaseEnemy(AEnemyPawn* Enemy);  

protected:  
   virtual void BeginPlay() override;

private:
	// ?癒?묋퉪袁⑥쨮 癰귣떯???롫뮉 ????
   TMap<FPrimaryAssetId, TArray<AEnemyPawn*>> EnemyPool;  

   // ??덉쨮 ??밴쉐?????????
   UPROPERTY(EditAnywhere)  
   TSubclassOf<AEnemyPawn> EnemyClass;

   // ??????곸뱽 ????덉쨮 ???紐꾨뮞??곷뮞 ??밴쉐
   AEnemyPawn* CreateNewEnemy(const FPrimaryAssetId& AssetId, USplineComponent* InSpline, float HealthMultiplier, float SpeedMultiplier, bool bIsBoss, FVector Scale);
};
