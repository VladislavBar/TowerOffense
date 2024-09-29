#include "DelayedStartWidget.h"

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

	ATowerOffenseGameMode* TowerOffenseGameMode = Cast<ATowerOffenseGameMode>(UGameplayStatics::GetGameMode(World));
	if (!IsValid(TowerOffenseGameMode)) return;

	OnDelayRemainingTimeDelegate.BindUObject(this, &UDelayedStartWidget::UpdateText);
	OnDelayRemainingTimeDelegateHandle = TowerOffenseGameMode->AddDelayRemainingTimeHandler(OnDelayRemainingTimeDelegate);
}