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

   // ������ �ƴ� Ŭ���̾�Ʈ������ ƽ ��Ȱ��
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

   // ���ö����� ������ �н�
   if (!Spline) return;

   // �����ڿ� ���� ������Ʈ ��������
   AEnemyPawn* OwnerPawn = Cast<AEnemyPawn>(GetOwner());

   if (!OwnerPawn || !OwnerPawn->GetStatus()) return;

   UEnemyStatusComponent* Status = OwnerPawn->GetStatus();

   // ���� ���¸� �̵� ����
   if (Status->IsStunned())
   {
       return;  // ���� ���� ������ ����
   }

   // ���ο� ���� ���� �ӵ� ���
   float EffectiveSpeed = Status->GetEffectiveMoveSpeed();

   CurrentDistance += EffectiveSpeed * DeltaTime;

   // ���� ó��
   const float SplineLength = Spline->GetSplineLength();
   if (CurrentDistance >= SplineLength - EndTolerance)
   {
       OwnerPawn->ReachGoal();
       return;
   }

   // ��ġ/ȸ�� ����
   FVector Location = Spline->GetLocationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);
   FVector Tangent = Spline->GetTangentAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);

   OwnerPawn->SetActorLocation(Location);
   OwnerPawn->SetActorRotation(Tangent.Rotation());

   // �ִϸ��̼� �ӵ��� EffectiveSpeed ����
   if (USkeletalMeshComponent* MeshComp = OwnerPawn->FindComponentByClass<USkeletalMeshComponent>())
   {
       if (UEnemyAnimInstance* EnemyAnim = Cast<UEnemyAnimInstance>(MeshComp->GetAnimInstance()))
       {
           EnemyAnim->SetSpeed(EffectiveSpeed);
       }
   }
}
