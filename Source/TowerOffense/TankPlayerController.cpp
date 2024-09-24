#include "TankPlayerController.h"

DEFINE_LOG_CATEGORY(LogTankPawn)

void ATankPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetupTankHUD();
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
	if (!IsValid(TankPawnHUDClass))
	{
		UE_LOG(LogTankPawn, Error, TEXT("TankPawnHUDClass is not set in %s"), *GetName());

		if (!IsValid(GEngine)) return;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("CooldownWidgetClass is not set in TankPlayerController"));

		return;
	}

	TankPawnHUD = CreateWidget<UTankPawnHUD>(this, TankPawnHUDClass);
	if (!IsValid(TankPawnHUD)) return;
	
	TankPawnHUD->AddToViewport();
}