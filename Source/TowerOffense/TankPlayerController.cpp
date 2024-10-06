#include "TankPlayerController.h"

#include "TankPawn.h"
#include "TowerOffenseGameMode.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogTankPawnController)

void ATankPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetupTankHUD();
	SetupDelegates();
}

void ATankPlayerController::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	ResetCursor();
}

void ATankPlayerController::ResetCursor()
{
	int32 ViewportWidth = 0;
	int32 ViewportHeight = 0;
	GetViewportSize(ViewportWidth, ViewportHeight);

	FVector2D MousePosition = FVector2D::ZeroVector;
	GetMousePosition(MousePosition.X, MousePosition.Y);
	SetMouseLocation(ViewportWidth / 2, ViewportHeight / 2);
}

void ATankPlayerController::SetupTankHUD()
{
	SetupHUD(TankPawnHUDClass, TankPawnHUD, GET_MEMBER_NAME_CHECKED(ATankPlayerController, TankPawnHUD));
	ClearEndScreenHUD();
	SetActorTickEnabled(true);
	SetShowMouseCursor(false);
	ResumeGame();
	SetInputMode(FInputModeGameOnly());

	if (IsValid(TankPawnHUD))
	{
		TankPawnHUD->SetupWidgets();
	}
}

void ATankPlayerController::SetupWinScreen()
{
	SetupHUD(WinScreenClass, WinScreen, GET_MEMBER_NAME_CHECKED(ATankPlayerController, WinScreen));
	ClearTankPawnHUD();
	SetActorTickEnabled(false);
	SetShowMouseCursor(true);
	PauseGame();
	SetInputMode(FInputModeUIOnly());
}

void ATankPlayerController::SetupLoseScreen()
{
	SetupHUD(LoseScreenClass, LoseScreen, GET_MEMBER_NAME_CHECKED(ATankPlayerController, LoseScreen));
	ClearTankPawnHUD();
	SetActorTickEnabled(false);
	SetShowMouseCursor(true);
	PauseGame();
	SetInputMode(FInputModeUIOnly());
}

ATowerOffenseGameMode* ATankPlayerController::GetTowerOffenseGameMode() const
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return nullptr;

	return Cast<ATowerOffenseGameMode>(UGameplayStatics::GetGameMode(World));
}

void ATankPlayerController::SetupDelegates()
{
	SetupOnLoseDelegate();
	SetupOnWinDelegate();
	SetupOnTowerOffenseGameModeLoadedDelegate();
	SetupOnTowerOffenseGameModeStartedDelegate();
	SetupOnAmmoReplenishStartsDelegate();
	SetupOnAmmoReplenishFinishesDelegate();
}

void ATankPlayerController::SetupOnLoseDelegate()
{
	ATowerOffenseGameMode* TowerOffenseGameMode = GetTowerOffenseGameMode();
	if (!IsValid(TowerOffenseGameMode)) return;

	OnPlayerLosesDelegate.BindUObject(this, &ATankPlayerController::OnPlayerLoses);
	OnPlayersLosesDelegateHandle = TowerOffenseGameMode->AddPlayerLosesHandler(OnPlayerLosesDelegate);
}

void ATankPlayerController::SetupOnWinDelegate()
{
	ATowerOffenseGameMode* TowerOffenseGameMode = GetTowerOffenseGameMode();
	if (!IsValid(TowerOffenseGameMode)) return;

	OnPlayerWinDelegate.BindUObject(this, &ATankPlayerController::OnPlayerWins);
	OnPlayerWinsDelegateHandle = TowerOffenseGameMode->AddPlayerWinsHandler(OnPlayerWinDelegate);
}

void ATankPlayerController::SetupOnTowerOffenseGameModeLoadedDelegate()
{
	ATowerOffenseGameMode* TowerOffenseGameMode = GetTowerOffenseGameMode();
	if (!IsValid(TowerOffenseGameMode)) return;

	OnTowerOffenseGameModeLoadedDelegate.BindUObject(this, &ATankPlayerController::SetupTankHUD);
	OnTowerOffenseGameModeLoadedDelegateHandle = TowerOffenseGameMode->AddDelayStartHandler(OnTowerOffenseGameModeLoadedDelegate);
}

void ATankPlayerController::SetupOnTowerOffenseGameModeStartedDelegate()
{
	ATowerOffenseGameMode* TowerOffenseGameMode = GetTowerOffenseGameMode();
	if (!IsValid(TowerOffenseGameMode)) return;

	OnTowerOffenseGameModeStartedDelegate.BindUObject(this, &ATankPlayerController::SetupTankHUD);
	OnTowerOffenseGameModeStartedDelegateHandle = TowerOffenseGameMode->AddDelayFinishHandler(OnTowerOffenseGameModeStartedDelegate);
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

void ATankPlayerController::PauseGame() const
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	UGameplayStatics::SetGamePaused(World, true);
}

void ATankPlayerController::ResumeGame() const
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	UGameplayStatics::SetGamePaused(World, false);
}