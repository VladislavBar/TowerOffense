#pragma once

#include "CoreMinimal.h"
#include "AmmoWidget.h"
#include "CooldownWidget.h"
#include "CrosshairWidget.h"
#include "DelayedStartWidget.h"
#include "EnemiesCountWidget.h"
#include "HealthWidget.h"
#include "Blueprint/UserWidget.h"
#include "TankPawnHUD.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTankPawnHUD, Log, All);

class UTankPawnHUD;

template <typename WidgetT>
using TMember = TObjectPtr<WidgetT> UTankPawnHUD::*;

UCLASS(Abstract)
class TOWEROFFENSE_API UTankPawnHUD : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditInstanceOnly, meta = (BindWidget))
	TObjectPtr<UCrosshairWidget> CrosshairWidget;

	UPROPERTY(EditInstanceOnly, meta = (BindWidget))
	TObjectPtr<UHealthWidget> HealthWidget;

	UPROPERTY(EditInstanceOnly, meta = (BindWidget))
	TObjectPtr<UCooldownWidget> CooldownWidget;

	UPROPERTY(EditInstanceOnly, meta = (BindWidget))
	TObjectPtr<UEnemiesCountWidget> EnemiesLeftWidget;

	UPROPERTY(EditInstanceOnly, meta = (BindWidget))
	TObjectPtr<UDelayedStartWidget> DelayedStartWidget;

	UPROPERTY(EditInstanceOnly, meta = (BindWidget))
	TObjectPtr<UAmmoWidget> AmmoWidget;

	UPROPERTY(EditAnywhere, Category = "HUD", meta = (ClampMin = "0.0"))
	float HealthBarHideTime = 5.f;

	UPROPERTY(EditAnywhere, Category = "HUD", meta = (ClampMin = "0.0"))
	float AmmoWidgetHideTime = 5.f;

	FOnAmmoChangedDelegate::FDelegate OnAmmoChangedDelegate;
	FOnAmmoReplenishStartsDelegate::FDelegate OnReplenishStartsDelegate;
	FOnAmmoReplenishFinishesDelegate::FDelegate OnReplenishFinishesDelegate;
	FOnHitTakenDelegate::FDelegate OnHitTakenDelegate;

	FDelegateHandle OnAmmoChangedDelegateHandle;
	FDelegateHandle OnReplenishStartsDelegateHandle;
	FDelegateHandle OnReplenishFinishesDelegateHandle;
	FDelegateHandle OnHitTakenDelegateHandle;

	FTimerHandle HealthBarHideTimerHandle;
	FTimerHandle AmmoWidgetHideTimerHandle;

	virtual void NativeConstruct() override;

	void SetupOnGamePreparing();
	void SetupOnGameStarted();
	void SetupDelegates();
	void SetupAmmoDelegates();
	void SetupHealthDelegates();

	void OnAmmoChanged(const int32 Ammo);
	void OnReplenishStarts();
	void OnReplenishFinishes(const int32 Ammo);
	void OnHitTaken(const FHitTakenData& HitTakenData);

	void HideHealthBar() const { HideComponent(&UTankPawnHUD::HealthWidget); };
	void HideAmmoWidget() const { HideComponent(&UTankPawnHUD::AmmoWidget); };

	template <typename T>
	void HideComponent(TMember<T> Member) const;

	template <typename T>
	void ShowComponent(TMember<T> Member) const;

	void ResetHealthBarHideTimer();
	void ResetAmmoWidgetHideTimer();
	void ClearAmmoWidgetHideTimer();

public:
	void SetupWidgets();
	void HideCooldownWidget() const { HideComponent(&UTankPawnHUD::CooldownWidget); };
	void ShowCooldownWidget() const { ShowComponent(&UTankPawnHUD::CooldownWidget); };
};

template <typename WidgetT>
void UTankPawnHUD::HideComponent(TMember<WidgetT> Member) const
{
	static_assert(TIsDerivedFrom<WidgetT, UUserWidget>::IsDerived, "Member must be a UUserWidget");
	if (!IsValid(this->*Member)) return;

	(this->*Member)->SetVisibility(ESlateVisibility::Hidden);
}

template <typename WidgetT>
void UTankPawnHUD::ShowComponent(TMember<WidgetT> Member) const
{
	static_assert(TIsDerivedFrom<WidgetT, UUserWidget>::IsDerived, "Member must be a UUserWidget");

	if (!IsValid(this->*Member)) return;

	(this->*Member)->SetVisibility(ESlateVisibility::Visible);
}
