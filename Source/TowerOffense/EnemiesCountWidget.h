#pragma once

#include "CoreMinimal.h"
#include "TowerOffenseGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "EnemiesCountWidget.generated.h"

UCLASS()
class TOWEROFFENSE_API UEnemiesCountWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditInstanceOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> EnemiesCountText;

	FOnEnemiesCountChanged::FDelegate OnEnemiesCountChangedDelegate;
	FDelegateHandle OnEnemiesCountChangedDelegateHandle;

	virtual void NativeConstruct() override;
	void UpdateText(int32 EnemiesCount);
	void SetupChangedEnemiesCountDelegate();
};