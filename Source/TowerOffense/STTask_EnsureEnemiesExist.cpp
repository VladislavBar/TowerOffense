#include "STTask_EnsureEnemiesExist.h"

#include "StateTreeExecutionContext.h"
#include "TowerOffenseGameState.h"
#include "Kismet/GameplayStatics.h"

EStateTreeRunStatus FSTTask_EnsureEnemiesExists::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!IsValid(InstanceData.AIController)) return EStateTreeRunStatus::Failed;

	const UWorld* World = Context.GetWorld();
	if (!IsValid(World)) return EStateTreeRunStatus::Failed;

	const ATowerOffenseGameState* TowerOffenseGameState = Cast<ATowerOffenseGameState>(World->GetGameState());
	if (!IsValid(TowerOffenseGameState)) return EStateTreeRunStatus::Failed;

	const AAIController* AIController = InstanceData.AIController;
	if (!IsValid(AIController)) return EStateTreeRunStatus::Failed;

	const ATurretPawn* TurretPawn = Cast<ATurretPawn>(AIController->GetPawn());
	if (!IsValid(TurretPawn)) return EStateTreeRunStatus::Failed;

	TArray<AActor*> Enemies{TowerOffenseGameState->GetActiveEnemies(TurretPawn->GetTeam())};
	InstanceData.Enemies.Array = Enemies;

	return EStateTreeRunStatus::Succeeded;
}
