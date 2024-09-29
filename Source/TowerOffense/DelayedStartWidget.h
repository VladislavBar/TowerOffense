#pragma once

#include "CoreMinimal.h"
#include "TowerOffenseGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "DelayedStartWidget.generated.h"

UCLASS(Abstract)
class TOWEROFFENSE_API UDelayedStartWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DelayedStartText;

	FOnDelayRemainingTimeDelegate::FDelegate OnDelayRemainingTimeDelegate;
	FDelegateHandle OnDelayRemainingTimeDelegateHandle;

	virtual void NativeConstruct() override;
	void UpdateText(float RemainingTime);
	void SetupOnDelayRemainingTimeDelegate();
};
