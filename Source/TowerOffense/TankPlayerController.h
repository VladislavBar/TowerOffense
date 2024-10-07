#pragma once

#include "CoreMinimal.h"
#include "EndScreenHUD.h"
#include "TankPawnHUD.h"
#include "TowerOffenseGameMode.h"
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

	FPlayerWinsDelegate::FDelegate OnPlayerWinDelegate;
	FPlayerLosesDelegate::FDelegate OnPlayerLosesDelegate;
	FOnDelayStartDelegate::FDelegate OnTowerOffenseGameModeLoadedDelegate;
	FOnDelayFinishDelegate::FDelegate OnTowerOffenseGameModeStartedDelegate;
	FOnAmmoReplenishStartsDelegate::FDelegate OnReplenishStartsDelegate;
	FOnAmmoReplenishFinishesDelegate::FDelegate OnReplenishFinishesDelegate;

	FDelegateHandle OnPlayerWinsDelegateHandle;
	FDelegateHandle OnPlayersLosesDelegateHandle;
	FDelegateHandle OnTowerOffenseGameModeLoadedDelegateHandle;
	FDelegateHandle OnTowerOffenseGameModeStartedDelegateHandle;
	FDelegateHandle OnAmmoReplenishStartsDelegateHandle;
	FDelegateHandle OnAmmoReplenishFinishesDelegateHandle;

	virtual void BeginPlay() override;
	virtual void Tick(const float DeltaSeconds) override;
	void ResetCursor();

	template <typename WidgetT>
	void SetupHUD(const TSubclassOf<WidgetT>& WidgetClass, TObjectPtr<WidgetT>& Widget, FName&& WidgetClassName);
	void SetupTankHUD();
	void SetupWinScreen();
	void SetupLoseScreen();

	ATowerOffenseGameMode* GetTowerOffenseGameMode() const;

	void SetupDelegates();
	void SetupOnLoseDelegate();
	void SetupOnWinDelegate();
	void SetupOnTowerOffenseGameModeLoadedDelegate();
	void SetupOnTowerOffenseGameModeStartedDelegate();
	void SetupOnAmmoReplenishStartsDelegate();
	void SetupOnAmmoReplenishFinishesDelegate();

	void OnPlayerWins();
	void OnPlayerLoses();

	void ClearTankPawnHUD() const;
	void ClearEndScreenHUD() const;
	void ClearWinScreenHUD() const;
	void ClearLoseScreenHUD() const;

	void HideCooldownWidget() const;
	void OnReplenishFinishes(const int32 NewAmmo) const;
	void ShowCooldownWidget() const;

	void PauseGame() const;
	void ResumeGame() const;
};

template <typename WidgetT>
void ATankPlayerController::SetupHUD(const TSubclassOf<WidgetT>& WidgetClass, TObjectPtr<WidgetT>& Widget, FName&& WidgetClassName)
{
	static_assert(TIsDerivedFrom<WidgetT, UUserWidget>::IsDerived, "WidgetT must be derived from UUserWidget");

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