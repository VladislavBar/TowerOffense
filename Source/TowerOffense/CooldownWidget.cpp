#include "CooldownWidget.h"

#include "TankPawn.h"

void UCooldownWidget::SetCooldownText(const float RemainingCooldownTime)
{
	if (!IsValid(CooldownText)) return;

	if (RemainingCooldownTime <= KINDA_SMALL_NUMBER)
	{
		CooldownText->SetText(FText::FromString("Ready to shoot!"));
		CooldownText->SetColorAndOpacity(FColor::Green);
	}
	else
	{
		const FString CooldownMessage = FString::Printf(TEXT("Cooldown: %.1f"), RemainingCooldownTime);
		CooldownText->SetText(FText::FromString(CooldownMessage));
		CooldownText->SetColorAndOpacity(FColor::Red);
	}
}

void UCooldownWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetupOnCooldownTickDelegate();
}

void UCooldownWidget::SetupOnCooldownTickDelegate()
{
	ATankPawn* Owner = Cast<ATankPawn>(GetOwningPlayerPawn());
	if (!IsValid(Owner)) return;

	Owner->OnCooldownTickDelegate.AddUObject(this, &UCooldownWidget::SetCooldownText);
}
