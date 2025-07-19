#include "Components/PolyPalsGamePawn/BuildTowerComponent.h"
#include "Components/PolyPalsGamePawn/TowerHandleComponent.h"
#include "PolyPalsGamePawn.h"
#include "PolyPalsController.h"
#include "PolyPalsPlayerState.h"
#include "Tower/TowerPropertyData.h"
#include "PreviewBuilding.h"
#include "PlacedTower.h"
#include "Subsystems/TowerDataManager.h"

#include "UI/TowerUpgradeWidget.h" 
#include "Blueprint/UserWidget.h"

#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerController.h"


UBuildTowerComponent::UBuildTowerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBuildTowerComponent::BeginPlay()
{
	Super::BeginPlay();

	TowerDataManager = GetWorld()->GetSubsystem<UTowerDataManager>();

	if (!GetOwner()->HasAuthority())
	{
		SpawnPreviewBuilding();
	}
}

void UBuildTowerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBuildTowerComponent, PlayerColor);
}

void UBuildTowerComponent::ClientBeginSelectTower_Implementation(uint8 InTowerId)
{
    UE_LOG(LogTemp, Warning, TEXT("BuildTowerComponent:Clinent 선택된 타워 ID = %d"), InTowerId);
    OnSelectTower(InTowerId);
}

void UBuildTowerComponent::OnSelectTower(uint8 InTowerId)
{
    UE_LOG(LogTemp, Warning, TEXT("BuildTowerComponent:OnSelectTower ID = %d"), InTowerId);

    if (!TowerDataManager)
    {
        UE_LOG(LogTemp, Error, TEXT("OnSelectTower: TowerDataManager is null"));
        return;
    }

    APolyPalsGamePawn* OwnerPawn = Cast<APolyPalsGamePawn>(GetOwner());

    if (OwnerPawn)
    {
        APolyPalsController* OwnerController = Cast<APolyPalsController>(OwnerPawn->GetController());

        if (OwnerController)
        {
            APolyPalsPlayerState* PS = Cast<APolyPalsPlayerState>(OwnerController->PlayerState);

            if (PS)
            {
                int32 Cost = TowerDataManager->GetTowerCost(InTowerId, ELevelValue::Level1);

                if (PS->GetPlayerGold() < Cost)
                {
                    UE_LOG(LogTemp, Warning, TEXT("Not enough gold to build tower %d"), InTowerId);

                    if (TryBuildButNotEnoughGold.IsBound())
                    {
                        TryBuildButNotEnoughGold.Execute();
                    }
                    return;
                }
            }
        }
    }

    // PreviewBuilding 이 아직 없으면 스폰 시도
    if (!PreviewBuilding)
    {
        SpawnPreviewBuilding();

        if (!PreviewBuilding)
        {
            UE_LOG(LogTemp, Error, TEXT("OnSelectTower: PreviewBuilding 스폰에 실패했습니다."));
            return;
        }
    }

    // 선택된 타워 ID 저장 및 상태 전환
    TowerOnSearchingQue = InTowerId;
    SetBuildState(EBuildState::SearchingPlace);
}

void UBuildTowerComponent::OnNormal()
{
    if (PreviewBuilding)
        PreviewBuilding->ShowPreviewBuilding(false);
}

void UBuildTowerComponent::OnSerchingPlace()
{
    UE_LOG(LogTemp, Warning, TEXT("BuildTowerComponent:OnSerchingPlace"));

    if (PreviewBuilding)
    {
        UE_LOG(LogTemp, Warning, TEXT("BuildTowerComponent:OnSerchingPlace Start"));
        PreviewBuilding->ShowPreviewBuilding(true, TowerOnSearchingQue);
    }
}

void UBuildTowerComponent::OnDecidePlacementLocation()
{
    if (!PreviewBuilding) return;

    bool bCanBuild = PreviewBuilding->IsBuildable();

    UE_LOG(LogTemp, Warning, TEXT("OnDecidePlacementLocation → bCanBuild=%d, Loc=%s"),  bCanBuild, *PreviewBuilding->GetActorLocation().ToString());

    if (bCanBuild)
    {
        UE_LOG(LogTemp, Warning, TEXT("Decide→Server RPC 호출"));
        Server_RequestSpawnTower(PreviewBuilding->GetActorLocation(), TowerOnSearchingQue);
        SetBuildState(EBuildState::None);
    }
    else
    {
        SetBuildState(EBuildState::SearchingPlace);
    }
}

void UBuildTowerComponent::SetBuildState(EBuildState InState)
{
    UE_LOG(LogTemp, Warning, TEXT("BuildTowerComponent:SetBuildState"));

    BuildState = InState;

    switch (InState)
    {
    case EBuildState::None:
        OnNormal();
        break;
    case EBuildState::SearchingPlace: 
        OnSerchingPlace(); 
        break;
    case EBuildState::DecidePlacementLocation: 
        OnDecidePlacementLocation(); 
        break;
    }
}

void UBuildTowerComponent::OnInputLeftClick()
{
    UE_LOG(LogTemp, Warning, TEXT("OnInputLeftClick (BuildState=%s)"), *UEnum::GetValueAsString(BuildState));

    if (BuildState == EBuildState::SearchingPlace)
    {
        SetBuildState(EBuildState::DecidePlacementLocation);
    }
}

void UBuildTowerComponent::OnInputRightClick()
{
    UE_LOG(LogTemp, Warning, TEXT("BuildTowerComponent:OnInputRightClick"));

    if (BuildState == EBuildState::SearchingPlace)
    {
        SetBuildState(EBuildState::None);
    }
}

void UBuildTowerComponent::OnInputTower1()
{
    UE_LOG(LogTemp, Warning, TEXT("BuildTowerComponent:OnInputTower1"));

    if (BuildState == EBuildState::None)
        OnSelectTower(1);
}

void UBuildTowerComponent::OnInputTower2()
{
    UE_LOG(LogTemp, Warning, TEXT("BuildTowerComponent:OnInputTower2"));

    if (BuildState == EBuildState::None)
        OnSelectTower(2);
}

void UBuildTowerComponent::OnInputTower3()
{
    UE_LOG(LogTemp, Warning, TEXT("BuildTowerComponent:OnInputTower3"));

    if (BuildState == EBuildState::None)
        OnSelectTower(3);
}

void UBuildTowerComponent::SpawnPreviewBuilding()
{
    UE_LOG(LogTemp, Warning, TEXT("BuildTowerComponent:tSpawnPreviewBuilding"));

	if (PreviewBuilding) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = GetOwner()->GetInstigator();

	PreviewBuilding = GetWorld()->SpawnActor<APreviewBuilding>(
		APreviewBuilding::StaticClass(),
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		SpawnParams
	);

	PreviewBuilding->ShowPreviewBuilding(false);
	PreviewBuilding->SetReplicates(false);
}

FVector UBuildTowerComponent::GetPreviewLocation() const
{
	return PreviewBuilding ? PreviewBuilding->GetActorLocation() : FVector::ZeroVector;
}

bool UBuildTowerComponent::Server_RequestSpawnTower_Validate(FVector_NetQuantize InLocation, uint8 InTargetTower)
{
    UE_LOG(LogTemp, Warning, TEXT("Server_RequestSpawnTower_Validate: Tower=%d"), InTargetTower);
    return InTargetTower >= 1 && InTargetTower <= 3;
}

void UBuildTowerComponent::Server_RequestSpawnTower_Implementation(FVector_NetQuantize InLocation, uint8 InTargetTower)
{
    UE_LOG(LogTemp, Warning, TEXT("[Server_Impl] Tower=%d Loc=%s"), InTargetTower, *InLocation.ToString());

    // Spawn 파라미터 세팅
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    // 실제 위치 오프셋
    FVector SpawnLocation = InLocation;
    SpawnLocation.Z += 80.f;

    // 타워 스폰
    APlacedTower* Tower = GetWorld()->SpawnActor<APlacedTower>(
        PlacedTowerBlueClass,
        SpawnLocation,
        FRotator::ZeroRotator,
        SpawnParams);

    // 내 Pawn 과 컨트롤러 얻기
    APolyPalsGamePawn* MyPawn = Cast<APolyPalsGamePawn>(GetOwner());
    if (!MyPawn)
        return;

    APolyPalsController* OwnerController =
        Cast<APolyPalsController>(MyPawn->GetController());
    if (!OwnerController)
        return;

    // 초기화 (색상·소유자 전달)
    Tower->ExternalInitializeTower(InTargetTower, PlayerColor, OwnerController);

    // 골드 차감
    if (APolyPalsPlayerState* PS = Cast<APolyPalsPlayerState>(OwnerController->PlayerState))
    {
        UTowerPropertyData* Data = GetWorld()->GetSubsystem<UTowerDataManager>()->GetTowerPropertyData(InTargetTower);

        if (!Data)
        {
            UE_LOG(LogTemp, Error, TEXT("UBuildTowerComponent: TowerDataManager is null!"));
            return;
        }

        int32 Cost = Data->UpgradeData[ELevelValue::Level1].Cost;

        PS->AddGold(-Cost);
    }
}