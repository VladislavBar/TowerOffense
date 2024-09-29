#include "TankPawnHUD.h"

void UTankPawnHUD::NativeConstruct()
{
	Super::NativeConstruct();

	SetupWidgets();
}

void UTankPawnHUD::SetupWidgets()
{
	const APawn* PlayerPawn = GetOwningPlayerPawn();
	if (!IsValid(PlayerPawn)) return;

	if (PlayerPawn->IsActorTickEnabled())
	{
		SetupOnGameStarted();
	}
	else
	{
		SetupOnGamePreparing();
	}
}

void UTankPawnHUD::SetupOnGamePreparing()
{
	if (IsValid(EnemiesLeftWidget))
	{
		EnemiesLeftWidget->SetVisibility(ESlateVisibility::Hidden);
	}

	if (IsValid(EnemiesLeftWidget))
	{
		DelayedStartWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void UTankPawnHUD::SetupOnGameStarted()
{
	if (IsValid(EnemiesLeftWidget))
	{
		EnemiesLeftWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

	if (IsValid(EnemiesLeftWidget))
	{
		DelayedStartWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}
