// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower/Components/TowerUpgradeComponent.h"

UTowerUpgradeComponent::UTowerUpgradeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTowerUpgradeComponent::BeginPlay()
{
	Super::BeginPlay();
}

//void UTowerUpgradeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//}

