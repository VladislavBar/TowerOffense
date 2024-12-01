#include "EnemyTowerAIController.h"

#include "TowerOffenseGameState.h"
#include "GameFramework/Pawn.h"
#include "Components/StateTreeComponent.h"

#include "Kismet/GameplayStatics.h"

AEnemyTowerAIController::AEnemyTowerAIController()
{
	StateTreeComponent = CreateDefaultSubobject<UStateTreeComponent>(TEXT("StateTreeComponent"));
	StateTreeComponent->SetStartLogicAutomatically(false);
}

void AEnemyTowerAIController::BeginPlay()
{
	Super::BeginPlay();

	SyncStateTreeToTickableState();
}

void AEnemyTowerAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	SetupOnPossessedPawnTickableDelegate();
	SubscribeToPlayersAmountChangedEvent();
}

void AEnemyTowerAIController::SetActorTickEnabled(bool bEnabled)
{
	Super::SetActorTickEnabled(bEnabled);
	SyncStateTreeToTickableState();
}

void AEnemyTowerAIController::RunStateTree() const
{
	if (!IsValid(StateTreeComponent)) return;
	StateTreeComponent->StartLogic();
}

void AEnemyTowerAIController::StopStateTree(const FString& Reason) const
{
	if (!IsValid(StateTreeComponent)) return;
	StateTreeComponent->StopLogic(Reason);
}

void AEnemyTowerAIController::SyncStateTreeToTickableState() const
{
	if (!IsValid(StateTreeComponent)) return;

	const APawn* PossessedPawn = GetPawn();
	if (!IsValid(PossessedPawn)) return;

	const bool IsStateTreeRunning = StateTreeComponent->IsRunning();
	const bool ShouldStateTreeRun = PossessedPawn->IsActorTickEnabled();

	if (IsStateTreeRunning == ShouldStateTreeRun) return;
	if (ShouldStateTreeRun) return RunStateTree();
	return StopStateTree(TEXT("State Tree should not run"));
}

void AEnemyTowerAIController::SubscribeToPlayersAmountChangedEvent()
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	ATowerOffenseGameState* TowerOffenseGameState = Cast<ATowerOffenseGameState>(World->GetGameState());
	if (!IsValid(TowerOffenseGameState)) return;

	OnParticipantAmountChangedDelegate.BindUObject(this, &AEnemyTowerAIController::OnParticipantsAmountChanged);
	OnPlayersAmountChangedDelegateHandle = TowerOffenseGameState->AddPlayersAmountChangedHandler(OnParticipantAmountChangedDelegate);
}

void AEnemyTowerAIController::UnsubscribeFromPLayersAmountChangedEvent() const
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	ATowerOffenseGameState* TowerOffenseGameState = Cast<ATowerOffenseGameState>(World->GetGameState());
	if (!IsValid(TowerOffenseGameState)) return;

	TowerOffenseGameState->RemovePlayersAmountChangedHandler(OnPlayersAmountChangedDelegateHandle);
}

void AEnemyTowerAIController::SetupOnPossessedPawnTickableDelegate()
{
	ATurretPawn* PossessedPawn = Cast<ATurretPawn>(GetPawn());
	if (!IsValid(PossessedPawn)) return;

	OnActorTickableEnabledDelegate.BindUObject(this, &AEnemyTowerAIController::OnPossessedPawnTickableEnabled);
	OnPossessedPawnTickableEnabledDelegateHandle = PossessedPawn->AddOnActorTickableEnabledHandler(OnActorTickableEnabledDelegate);
}

void AEnemyTowerAIController::ClearOnPossessedPawnTickableDelegate()
{
	ATurretPawn* PossessedPawn = Cast<ATurretPawn>(GetPawn());
	if (!IsValid(PossessedPawn)) return;

	PossessedPawn->RemoveOnActorTickableEnabledHandler(OnPossessedPawnTickableEnabledDelegateHandle);
	OnPossessedPawnTickableEnabledDelegateHandle.Reset();
}

void AEnemyTowerAIController::OnPossessedPawnTickableEnabled(const bool bEnabled) const
{
	SyncStateTreeToTickableState();
}

void AEnemyTowerAIController::OnParticipantsAmountChanged(const TArray<ATurretPawn*>& Participants) const
{
	if (!IsValid(StateTreeComponent) || !ResetEnemiesEvent.Tag.IsValid()) return;

	StateTreeComponent->SendStateTreeEvent(ResetEnemiesEvent);
}