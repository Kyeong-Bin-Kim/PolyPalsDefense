#include "PolyPalsGameInstance.h"
#include "PolyPalsController.h"
#include "PolyPalsPlayerState.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Kismet/GameplayStatics.h"

void UPolyPalsGameInstance::Init()
{
    Super::Init();

    // World 컨텍스트 기반 OnlineSubsystem 가져오기
    IOnlineSubsystem* OSS = Online::GetSubsystem(GetWorld());

    if (OSS)
    {
        IdentityInterface = OSS->GetIdentityInterface();
        UE_LOG(LogTemp, Log, TEXT("Init(): Using OSS from world, Interface 초기화 완료"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Init(): World OSS를 찾을 수 없습니다."));
    }

    // 델리게이트 바인딩
    if (IdentityInterface.IsValid())
    {
        OnLoginCompleteHandle = IdentityInterface->AddOnLoginCompleteDelegate_Handle(
            0,
            FOnLoginCompleteDelegate::CreateUObject(this, &UPolyPalsGameInstance::OnLoginComplete)
        );
    }
}

void UPolyPalsGameInstance::LoginToSteam()
{
    if (IdentityInterface.IsValid())
    {
        FOnlineAccountCredentials Credentials;
        IdentityInterface->Login(0, Credentials);
    }
}

void UPolyPalsGameInstance::OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
    UE_LOG(LogTemp, Log, TEXT("Steam login %s"), bWasSuccessful ? TEXT("succeeded") : TEXT("failed"));
    if (bWasSuccessful)
    {
        FString PlayerName = IdentityInterface.IsValid() ? IdentityInterface->GetPlayerNickname(UserId) : TEXT("");

        if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
        {
            if (APolyPalsController* PPC = Cast<APolyPalsController>(PC))
            {
                PPC->UpdatePlayerNickname(PlayerName);
            }
        }
    }
}