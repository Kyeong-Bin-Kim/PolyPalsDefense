#include "UI/TowerLevelWidget.h"
#include "Components/Image.h"

void UTowerLevelWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UpdateLevelUI(1); // 기본 테스트용
}

void UTowerLevelWidget::UpdateLevelUI(int32 CurrentLevel)
{
	if (!FilledStarTexture || !EmptyStarTexture) return;

	TArray<UImage*> StarImages = { Star1Image, Star2Image, Star3Image };

	for (int32 i = 0; i < StarImages.Num(); ++i)
	{
		if (StarImages[i])
		{
			StarImages[i]->SetBrushFromTexture(i < CurrentLevel ? FilledStarTexture : EmptyStarTexture);
		}
	}
}
