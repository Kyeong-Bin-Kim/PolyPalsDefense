#include "Enemy/EnemySplineMovementComponent.h"  
#include "Components/SplineComponent.h"  
#include "EnemyAnimInstance.h"  
#include "GameFramework/Actor.h"  
#include "Components/SkeletalMeshComponent.h" 
#include "EnemyPawn.h"
#include "EnemyStatusComponent.h"

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

   // 서버가 아닌 클라이언트에서는 틱 비활성
   if (!GetOwner()->HasAuthority())
   {
       PrimaryComponentTick.SetTickFunctionEnable(false);
   }
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

   // 스플라인이 없으면 패스
   if (!Spline) return;

   // 소유자와 상태 컴포넌트 가져오기
   AEnemyPawn* OwnerPawn = Cast<AEnemyPawn>(GetOwner());

   if (!OwnerPawn || !OwnerPawn->GetStatus()) return;

   UEnemyStatusComponent* Status = OwnerPawn->GetStatus();

   // 스턴 상태면 이동 중지
   if (Status->IsStunned())
   {
       return;  // 스턴 해제 전까지 멈춤
   }

   // 슬로우 비율 적용 속도 계산
   float EffectiveSpeed = Status->GetEffectiveMoveSpeed();

   CurrentDistance += EffectiveSpeed * DeltaTime;

   // 도착 처리
   const float SplineLength = Spline->GetSplineLength();
   if (CurrentDistance >= SplineLength - EndTolerance)
   {
       OwnerPawn->ReachGoal();
       return;
   }

   // 위치/회전 갱신
   FVector Location = Spline->GetLocationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);
   FVector Tangent = Spline->GetTangentAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);

   OwnerPawn->SetActorLocation(Location);
   OwnerPawn->SetActorRotation(Tangent.Rotation());

   // 애니메이션 속도에 EffectiveSpeed 전달
   if (USkeletalMeshComponent* MeshComp = OwnerPawn->FindComponentByClass<USkeletalMeshComponent>())
   {
       if (UEnemyAnimInstance* EnemyAnim = Cast<UEnemyAnimInstance>(MeshComp->GetAnimInstance()))
       {
           EnemyAnim->SetSpeed(EffectiveSpeed);
       }
   }
}
