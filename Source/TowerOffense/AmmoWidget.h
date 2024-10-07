#pragma once

#include "CoreMinimal.h"
#include "AmmoComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "AmmoWidget.generated.h"

UCLASS(Abstract)
class TOWEROFFENSE_API UAmmoWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditInstanceOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> AmmoText;

	FOnAmmoInitializedDelegate::FDelegate OnAmmoInitializedDelegate;
	FOnAmmoChangedDelegate::FDelegate OnAmmoChangedDelegate;
	FOnAmmoReplenishRemainingTimeDelegate::FDelegate OnAmmoReplenishRemainingTime;

	FDelegateHandle OnAmmoInitializedDelegateHandle;
	FDelegateHandle OnAmmoChangedDelegateHandle;
	FDelegateHandle OnAmmoReplenishRemainingTimeHandle;

	virtual void NativeConstruct() override;

	void SetAmmoText(const int32 Ammo);
	FText GetAmmoText(const int32 Ammo) const;
	void SetupDelegates();
	void DisplayReplenishAmmoRemainingTime(const float RemainingTime) const;
};
