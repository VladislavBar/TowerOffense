#include "TankPlayerController.h"

DEFINE_LOG_CATEGORY(LogTankPawn)

void ATankPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetupCooldownWidget();
	SetupCrosshairWidget();
}

void ATankPlayerController::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	ResetCursor();
	CrosshairFollowMouse();
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

void ATankPlayerController::CrosshairFollowMouse()
{
	if (!IsValid(CrosshairWidget)) return;

	FVector2D MousePosition = FVector2D::ZeroVector;
	GetMousePosition(MousePosition.X, MousePosition.Y);
	CrosshairWidget->SetPositionInViewport(MousePosition);
}

void ATankPlayerController::SetupCrosshairWidget()
{
	if (!IsValid(CrosshairWidgetClass))
	{
		UE_LOG(LogTankPawn, Error, TEXT("CrosshairWidgetClass is not set in %s"), *GetName());

		if (!IsValid(GEngine)) return;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("CrosshairWidgetClass is not set in TankPlayerController"));

		return;
	}

	CrosshairWidget = CreateWidget<UCrosshairWidget>(this, CrosshairWidgetClass);
	CrosshairWidget->AddToViewport();
}

void ATankPlayerController::SetupCooldownWidget()
{
	if (!IsValid(CooldownWidgetClass))
	{
		UE_LOG(LogTankPawn, Error, TEXT("CooldownWidgetClass is not set in %s"), *GetName());

		if (!IsValid(GEngine)) return;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("CooldownWidgetClass is not set in TankPlayerController"));

		return;
	}

	CooldownWidget = CreateWidget<UCooldownWidget>(this, CooldownWidgetClass);
	CooldownWidget->AddToViewport();
}

void ATankPlayerController::RefreshCooldownWidget(float RemainingCooldownTime)
{
	if(!IsValid(CooldownWidget)) return;

	CooldownWidget->SetCooldownText(RemainingCooldownTime);
}