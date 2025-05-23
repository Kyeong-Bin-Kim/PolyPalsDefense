// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower/TestEnemy.h"
#include "Components/SphereComponent.h"

// Sets default values
ATestEnemy::ATestEnemy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootSphere = CreateDefaultSubobject<USphereComponent>(TEXT("RootSphere"));
	SetRootComponent(RootSphere);
	
	EnemyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("EnemyMesh"));
	EnemyMesh->SetupAttachment(RootSphere);

	Tags.Add(FName("Enemy"));
}

// Called when the game starts or when spawned
void ATestEnemy::BeginPlay()
{
	Super::BeginPlay();

	EnemyMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	EnemyMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);
	//EnemyMesh->SetGenerateOverlapEvents(true);
}

// Called every frame
void ATestEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float ATestEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Log, TEXT("ATestEnemy take damage: %f"), Damage);
	}

	return Damage;
}

