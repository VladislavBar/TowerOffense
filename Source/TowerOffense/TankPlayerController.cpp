#include "TankPlayerController.h"

#include "TowerOffenseGameMode.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogTankPawnController)

void ATankPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetupTankHUD();
	SetupOnWinDelegate();
	SetupOnLoseDelegate();
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

void ATankPlayerController::SetupOnLoseDelegate()
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	ATowerOffenseGameMode* TowerOffenseGameMode = Cast<ATowerOffenseGameMode>(UGameplayStatics::GetGameMode(World));
	if (!IsValid(TowerOffenseGameMode)) return;

	OnPlayerLosesDelegate.BindUObject(this, &ATankPlayerController::OnPlayerLoses);
	OnPlayersLosesDelegateHandle = TowerOffenseGameMode->AddPlayerLosesHandler(OnPlayerLosesDelegate);
}

void ATankPlayerController::SetupOnWinDelegate()
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	ATowerOffenseGameMode* TowerOffenseGameMode = Cast<ATowerOffenseGameMode>(UGameplayStatics::GetGameMode(World));
	if (!IsValid(TowerOffenseGameMode)) return;

	OnPlayerWinDelegate.BindUObject(this, &ATankPlayerController::OnPlayerWins);
	OnPlayerWinsDelegateHandle = TowerOffenseGameMode->AddPlayerWinsHandler(OnPlayerWinDelegate);
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