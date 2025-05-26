#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PolyPalsHUD.generated.h"

class UGamePlayingUIWidget;

/**
 * 실제 게임 플레이 중 표시되는 HUD 클래스
 */
UCLASS()
class POLYPALSDEFENSE_API APolyPalsHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UGamePlayingUIWidget> GamePlayingWidgetClass;

private:
	UPROPERTY()
	UGamePlayingUIWidget* GamePlayingWidget;

public:
	FORCEINLINE UGamePlayingUIWidget* GetGamePlayingWidget() const { return GamePlayingWidget; }
};
