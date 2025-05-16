#include "Enemy/EnemySplineMovementComponent.h"  
#include "Components/SplineComponent.h"  
#include "EnemyAnimInstance.h"  
#include "GameFramework/Actor.h"  
#include "Components/SkeletalMeshComponent.h"  

UEnemySplineMovementComponent::UEnemySplineMovementComponent()  
{  
   PrimaryComponentTick.bCanEverTick = true;  
   bIsPaused = false;  
   CurrentDistance = 0.f;  
   MoveSpeed = 0.f;  
}  

void UEnemySplineMovementComponent::BeginPlay()  
{  
   Super::BeginPlay();  
}  

void UEnemySplineMovementComponent::Initialize(USplineComponent* InSpline, float InMoveSpeed)  
{  
   Spline = InSpline;  
   CurrentDistance = 0.f;  
   MoveSpeed = InMoveSpeed;  
   bIsPaused = false;  
}  

void UEnemySplineMovementComponent::SetMoveSpeed(float NewSpeed)  
{  
   MoveSpeed = NewSpeed;  
}  

void UEnemySplineMovementComponent::SetPaused(bool bPause)  
{  
   bIsPaused = bPause;  
}  

void UEnemySplineMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)  
{  
   Super::TickComponent(DeltaTime, TickType, ThisTickFunction);  

   if (!Spline || bIsPaused) return;  

   CurrentDistance += MoveSpeed * DeltaTime;  

   FVector Location = Spline->GetLocationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);  
   FVector Tangent = Spline->GetTangentAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);  

   AActor* Owner = GetOwner();  
   if (!Owner) return;  

   Owner->SetActorLocation(Location);  
   Owner->SetActorRotation(Tangent.Rotation());  

   USkeletalMeshComponent* MeshComp = Owner->FindComponentByClass<USkeletalMeshComponent>();  
   if (MeshComp)  
   {  
       if (UAnimInstance* Anim = MeshComp->GetAnimInstance())  
       {  
           if (UEnemyAnimInstance* EnemyAnim = Cast<UEnemyAnimInstance>(Anim))  
           {  
               EnemyAnim->SetSpeed(MoveSpeed);  
           }  
       }  
   }  
}
