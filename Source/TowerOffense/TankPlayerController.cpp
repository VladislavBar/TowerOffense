#include "TankPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "TankPawn.h"
#include "TowerOffenseGameState.h"
#include "TowerOffensePlayerState.h"
#include "GameFramework/HUD.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogTankPawnController)

void ATankPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetupTankHUD();
	SetupDelegates();
}

void ATankPlayerController::SetupTankHUD()
{
	SetupHUD(TankPawnHUDClass, TankPawnHUD, GET_MEMBER_NAME_CHECKED(ATankPlayerController, TankPawnHUD));
	ClearEndScreenHUD();
	SetShowMouseCursor(false);
	SetInputMode(FInputModeGameOnly());
}

void ATankPlayerController::SetupWinScreen()
{
	SetupHUD(WinScreenClass, WinScreen, GET_MEMBER_NAME_CHECKED(ATankPlayerController, WinScreen));
	ClearTankPawnHUD();
	SetShowMouseCursor(true);
	SetInputMode(FInputModeUIOnly());
}

void ATankPlayerController::SetupLoseScreen()
{
	SetupHUD(LoseScreenClass, LoseScreen, GET_MEMBER_NAME_CHECKED(ATankPlayerController, LoseScreen));
	ClearTankPawnHUD();
	SetShowMouseCursor(true);
	SetInputMode(FInputModeUIOnly());
}

ATowerOffenseGameState* ATankPlayerController::GetTowerOffenseGameState() const
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return nullptr;

	return Cast<ATowerOffenseGameState>(World->GetGameState());
}

void ATankPlayerController::OnCursorToggle(const FInputActionInstance& ActionValue)
{
	bShouldResetCursor = !bShouldResetCursor;
}

void ATankPlayerController::OnMatchWaitingToStart()
{
	SetupTankHUD();
	if (IsValid(TankPawnHUD))
	{
		TankPawnHUD->OnMatchWaitingToStart();
	}
}

void ATankPlayerController::OnMatchStarted()
{
	SetupTankHUD();
	if (IsValid(TankPawnHUD))
	{
		TankPawnHUD->OnMatchStarted();
	}
}

void ATankPlayerController::SetupDelegates()
{
	SetupOnTowerOffenseGameStateLoadedDelegate();
	SetupOnTowerOffenseGameStateStartedDelegate();
	SetupOnTowerOffenseGameStateFinishedDelegate();
	SetupOnAmmoReplenishStartsDelegate();
	SetupOnAmmoReplenishFinishesDelegate();
}

void ATankPlayerController::SetupOnTowerOffenseGameStateLoadedDelegate()
{
	ATowerOffenseGameState* TowerOffenseGameState = GetTowerOffenseGameState();
	if (!IsValid(TowerOffenseGameState)) return;

	OnTowerOffenseGameStateLoadedDelegate.BindUObject(this, &ATankPlayerController::OnMatchWaitingToStart);
	OnTowerOffenseGameStateLoadedDelegateHandle = TowerOffenseGameState->AddDelayStartHandler(OnTowerOffenseGameStateLoadedDelegate);
}

void ATankPlayerController::SetupOnTowerOffenseGameStateStartedDelegate()
{
	ATowerOffenseGameState* TowerOffenseGameState = GetTowerOffenseGameState();
	if (!IsValid(TowerOffenseGameState)) return;

	OnTowerOffenseGameStateStartedDelegate.BindUObject(this, &ATankPlayerController::OnMatchStarted);
	OnTowerOffenseGameStateStartedDelegateHandle = TowerOffenseGameState->AddMatchStartedHandler(OnTowerOffenseGameStateStartedDelegate);
}

void ATankPlayerController::SetupOnTowerOffenseGameStateFinishedDelegate()
{
	ATowerOffenseGameState* TowerOffenseGameState = GetTowerOffenseGameState();
	if (!IsValid(TowerOffenseGameState)) return;

	OnTowerOffenseGameStateFinishedDelegate.BindUObject(this, &ATankPlayerController::OnMatchFinished);
	OnTowerOffenseGameStateFinishedDelegateHandle = TowerOffenseGameState->AddMatchFinishedHandler(OnTowerOffenseGameStateFinishedDelegate);
}

void ATankPlayerController::SetupOnAmmoReplenishStartsDelegate()
{
	const ATankPawn* TankPawn = Cast<ATankPawn>(GetPawn());
	if (!IsValid(TankPawn)) return;

	UAmmoComponent* AmmoComponent = TankPawn->FindComponentByClass<UAmmoComponent>();
	if (!IsValid(AmmoComponent)) return;

	OnReplenishStartsDelegate.BindUObject(this, &ATankPlayerController::HideCooldownWidget);
	AmmoComponent->AddAmmoReplenishStarts(OnReplenishStartsDelegate);
}

void ATankPlayerController::SetupOnAmmoReplenishFinishesDelegate()
{
	const ATankPawn* TankPawn = Cast<ATankPawn>(GetPawn());
	if (!IsValid(TankPawn)) return;

	UAmmoComponent* AmmoComponent = TankPawn->FindComponentByClass<UAmmoComponent>();
	if (!IsValid(AmmoComponent)) return;

	OnReplenishFinishesDelegate.BindUObject(this, &ATankPlayerController::OnReplenishFinishes);
	AmmoComponent->AddAmmoReplenishFinishes(OnReplenishFinishesDelegate);
}

void ATankPlayerController::OnMatchFinished(ETeam WinningTeam)
{
	const ATowerOffensePlayerState* TowerOffensePlayerState = Cast<ATowerOffensePlayerState>(PlayerState);
	if (!IsValid(TowerOffensePlayerState)) return;

	if (TowerOffensePlayerState->GetTeam() == WinningTeam)
	{
		OnPlayerWins();
	}
	else
	{
		OnPlayerLoses();
	}
}

void ATankPlayerController::OnPlayerWins()
{
	ClearTankPawnHUD();
	SetupWinScreen();
}

void ATankPlayerController::OnPlayerLoses()
{
	ClearTankPawnHUD();
	SetupLoseScreen();
}

void ATankPlayerController::ClearTankPawnHUD() const
{
	if (!IsValid(TankPawnHUD)) return;

	TankPawnHUD->RemoveFromParent();
}

void ATankPlayerController::ClearEndScreenHUD() const
{
	ClearWinScreenHUD();
	ClearLoseScreenHUD();
}

void ATankPlayerController::ClearWinScreenHUD() const
{
	if (!IsValid(WinScreen)) return;

	WinScreen->RemoveFromParent();
}
void ATankPlayerController::ClearLoseScreenHUD() const
{
	if (!IsValid(LoseScreen)) return;

	LoseScreen->RemoveFromParent();
}

void ATankPlayerController::HideCooldownWidget() const
{
	if (!IsValid(TankPawnHUD)) return;

	TankPawnHUD->HideCooldownWidget();
}

void ATankPlayerController::OnReplenishFinishes(const int32 NewAmmo) const
{
	ShowCooldownWidget();
}

void ATankPlayerController::ShowCooldownWidget() const
{
	if (!IsValid(TankPawnHUD)) return;

	TankPawnHUD->ShowCooldownWidget();
}