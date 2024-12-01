#include "EnemiesCountWidget.h"

#include "TeamHelper.h"
#include "TowerOffenseGameState.h"
#include "Kismet/GameplayStatics.h"

void UEnemiesCountWidget::SetupChangedEnemiesCountDelegate()
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	ATowerOffenseGameState* TowerOffenseGameState = Cast<ATowerOffenseGameState>(World->GetGameState());
	if (!IsValid(TowerOffenseGameState)) return;

	OnParticipantsAmountChangedDelegate.BindUObject(this, &UEnemiesCountWidget::OnParticipantsAmountChanged);
	OnParticipantsAmountChangedDelegateHandle = TowerOffenseGameState->AddPlayersAmountChangedHandler(OnParticipantsAmountChangedDelegate);
}

void UEnemiesCountWidget::OnParticipantsAmountChanged(const TArray<ATurretPawn*>& Participants)
{
	const ATurretPawn* PlayerPawn = Cast<ATurretPawn>(GetOwningPlayerPawn());
	if (!IsValid(PlayerPawn)) return;

	const int32 EnemiesCount = FTeamHelper::GetEnemiesCount(Participants, PlayerPawn->GetTeam());
	UpdateText(EnemiesCount);
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
