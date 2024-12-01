#pragma once

#include <typeinfo>

#include "CoreMinimal.h"
#include "EndScreenHUD.h"
#include "InputMappingContext.h"
#include "TankPawnHUD.h"
#include "TowerOffenseGameState.h"
#include "GameFramework/PlayerController.h"
#include "TankPlayerController.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTankPawnController, Log, All);

UCLASS()
class TOWEROFFENSE_API ATankPlayerController : public APlayerController
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UTankPawnHUD> TankPawnHUDClass;

	UPROPERTY()
	TObjectPtr<UTankPawnHUD> TankPawnHUD;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UEndScreenHUD> WinScreenClass;

	UPROPERTY()
	TObjectPtr<UEndScreenHUD> WinScreen;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UEndScreenHUD> LoseScreenClass;

	UPROPERTY()
	TObjectPtr<UEndScreenHUD> LoseScreen;

	FOnDelayStartDelegate::FDelegate OnTowerOffenseGameStateLoadedDelegate;
	FDelegateHandle OnTowerOffenseGameStateLoadedDelegateHandle;

	FOnMatchStartedDelegate::FDelegate OnTowerOffenseGameStateStartedDelegate;
	FDelegateHandle OnTowerOffenseGameStateStartedDelegateHandle;

	FOnMatchFinished::FDelegate OnTowerOffenseGameStateFinishedDelegate;
	FDelegateHandle OnTowerOffenseGameStateFinishedDelegateHandle;

	FOnAmmoReplenishStartsDelegate::FDelegate OnReplenishStartsDelegate;
	FDelegateHandle OnAmmoReplenishStartsDelegateHandle;

	FOnAmmoReplenishFinishesDelegate::FDelegate OnReplenishFinishesDelegate;
	FDelegateHandle OnAmmoReplenishFinishesDelegateHandle;

	FOnParticipantsAmountChangedDelegate::FDelegate OnParticipantsAmountChangedDelegate;
	FDelegateHandle OnParticipantsAmountChangedDelegateHandle;

	bool bShouldResetCursor = true;
	bool bHasWindowFocus = true;

private:
	virtual void BeginPlay() override;

	template <typename WidgetT>
	void SetupHUD(const TSubclassOf<WidgetT>& WidgetClass, TObjectPtr<WidgetT>& Widget, FName&& WidgetClassName);
	void SetupTankHUD();
	void SetupWinScreen();
	void SetupLoseScreen();

	ATowerOffenseGameState* GetTowerOffenseGameState() const;

	void OnCursorToggle(const FInputActionInstance& ActionValue);
	void OnMatchWaitingToStart();
	void OnMatchStarted();

	void SetupDelegates();
	void SetupOnTowerOffenseGameStateLoadedDelegate();
	void SetupOnTowerOffenseGameStateStartedDelegate();
	void SetupOnTowerOffenseGameStateFinishedDelegate();
	void SetupOnAmmoReplenishStartsDelegate();
	void SetupOnAmmoReplenishFinishesDelegate();

	void OnMatchFinished(ETeam WinningTeam);
	void OnPlayerWins();
	void OnPlayerLoses();

	void ClearTankPawnHUD() const;
	void ClearEndScreenHUD() const;
	void ClearWinScreenHUD() const;
	void ClearLoseScreenHUD() const;

	void HideCooldownWidget() const;
	void OnReplenishFinishes(const int32 NewAmmo) const;
	void ShowCooldownWidget() const;
};

template <typename WidgetT>
void ATankPlayerController::SetupHUD(const TSubclassOf<WidgetT>& WidgetClass, TObjectPtr<WidgetT>& Widget, FName&& WidgetClassName)
{
	static_assert(TIsDerivedFrom<WidgetT, UUserWidget>::IsDerived, "WidgetT must be derived from UUserWidget");

	if (!IsLocalPlayerController()) return;

	if (IsValid(Widget) && Widget->IsInViewport()) return;
	if (IsValid(Widget) && !Widget->IsInViewport()) return Widget->AddToViewport();

	if (!IsValid(WidgetClass))
	{
		const FString ErrorMessage = FString::Printf(TEXT("%s is not set in %s"), *FString(typeid(WidgetT).name()), *WidgetClassName.ToString());
		UE_LOG(LogTankPawnController, Error, TEXT("%s"), *ErrorMessage);

		if (!IsValid(GEngine)) return;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, ErrorMessage);

		return;
	}

	Widget = CreateWidget<WidgetT>(this, WidgetClass);
	if (!IsValid(Widget)) return;

	Widget->AddToViewport();
}