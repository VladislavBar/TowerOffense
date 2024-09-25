#include "EnemiesCountWidget.h"

#include "Kismet/GameplayStatics.h"

void UEnemiesCountWidget::SetupChangedEnemiesCountDelegate()
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	ATowerOffenseGameMode* TowerOffenseGameMode = Cast<ATowerOffenseGameMode>(UGameplayStatics::GetGameMode(World));
	if (!IsValid(TowerOffenseGameMode)) return;

	OnEnemiesCountChangedDelegate.BindUObject(this, &UEnemiesCountWidget::UpdateText);
	OnEnemiesCountChangedDelegateHandle = TowerOffenseGameMode->AddEnemiesCountChangedHandler(OnEnemiesCountChangedDelegate);
}

void UEnemiesCountWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetupChangedEnemiesCountDelegate();
}

void UEnemiesCountWidget::UpdateText(int32 EnemiesCount)
{
	if (EnemiesCount > 1)
	{
		const FString EnemiesCountString = FString::Printf(TEXT("There are %d enemies left"), EnemiesCount);
		EnemiesCountText->SetText(FText::FromString(EnemiesCountString));
	}
	else if (EnemiesCount == 1)
	{
		EnemiesCountText->SetText(FText::FromString("There is 1 enemy left"));
	}
	else
	{
		EnemiesCountText->SetText(FText::FromString("No enemies left"));
	}
}
