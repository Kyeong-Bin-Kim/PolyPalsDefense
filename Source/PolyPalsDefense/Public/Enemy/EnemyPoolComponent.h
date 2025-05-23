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

   // 에셋 ID에 대응하는 적을 풀에서 꺼내거나 새로 생성
   AEnemyPawn* AcquireEnemy(const FPrimaryAssetId& AssetId, USplineComponent* InSpline, float HealthMultiplier, float SpeedMultiplier, bool bIsBoss, FVector Scale, float CollisionRadius);

   // 사용이 끝난 적을 풀로 반환
   void ReleaseEnemy(AEnemyPawn* Enemy);  

protected:  
   virtual void BeginPlay() override;

private:
	// 에셋별로 보관하는 적 풀
   TMap<FPrimaryAssetId, TArray<AEnemyPawn*>> EnemyPool;  

   // 새로 생성할 적 클래스
   UPROPERTY(EditAnywhere)  
   TSubclassOf<AEnemyPawn> EnemyClass;

   // 풀에 없을 시 새로 적 인스턴스 생성
   AEnemyPawn* CreateNewEnemy(const FPrimaryAssetId& AssetId, USplineComponent* InSpline, float HealthMultiplier, float SpeedMultiplier, bool bIsBoss, FVector Scale, float CollisionRadius);
};
