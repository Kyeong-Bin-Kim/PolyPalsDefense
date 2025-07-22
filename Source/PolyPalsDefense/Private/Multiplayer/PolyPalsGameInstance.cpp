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

    // World 而⑦뀓?ㅽ듃 湲곕컲 OnlineSubsystem 媛?몄삤湲?
    IOnlineSubsystem* OSS = Online::GetSubsystem(GetWorld());

    if (OSS)
    {
        IdentityInterface = OSS->GetIdentityInterface();
        UE_LOG(LogTemp, Log, TEXT("Init(): Using OSS from world, Interface 珥덇린???꾨즺"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Init(): World OSS瑜?李얠쓣 ???놁뒿?덈떎."));
    }

    // ?몃━寃뚯씠??諛붿씤??
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