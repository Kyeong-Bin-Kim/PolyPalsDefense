#pragma once

#include "CoreMinimal.h"
#include "LobbyInfo.generated.h"

USTRUCT(BlueprintType)
struct FLobbyInfo
{
    GENERATED_BODY();

    UPROPERTY(BlueprintReadOnly)
    FString SessionId;

    UPROPERTY(BlueprintReadOnly)
    FString LobbyName;

    UPROPERTY(BlueprintReadOnly)
    int32 CurrentPlayers = 0;

    UPROPERTY(BlueprintReadWrite)
    int32 ExpectedPlayerCount;

    UPROPERTY(BlueprintReadOnly)
    int32 MaxPlayers = 4;

    UPROPERTY(BlueprintReadOnly)
    bool bInProgress = false;
};