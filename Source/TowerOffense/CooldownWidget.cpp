#include "CooldownWidget.h"

void UCooldownWidget::SetCooldownText(float RemainingCooldownTime)
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

