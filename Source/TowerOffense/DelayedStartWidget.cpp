#include "DelayedStartWidget.h"

#include "TowerOffenseGameState.h"
#include "Kismet/GameplayStatics.h"

void UDelayedStartWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetupOnDelayRemainingTimeDelegate();
}

void UDelayedStartWidget::UpdateText(float RemainingTime)
{
	if (!IsValid(DelayedStartText)) return;

	DelayedStartText->SetText(FText::FromString(FString::Printf(TEXT("The game starts in: %.0f"), RemainingTime)));
}

void UDelayedStartWidget::SetupOnDelayRemainingTimeDelegate()
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	ATowerOffenseGameState* TowerOffenseGameState = Cast<ATowerOffenseGameState>(World->GetGameState());
	if (!IsValid(TowerOffenseGameState)) return;

	OnDelayRemainingTimeDelegate.BindUObject(this, &UDelayedStartWidget::UpdateText);
	OnDelayRemainingTimeDelegateHandle = TowerOffenseGameState->AddDelayRemainingTimeHandler(OnDelayRemainingTimeDelegate);
}