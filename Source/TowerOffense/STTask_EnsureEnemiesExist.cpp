#include "STTask_EnsureEnemiesExist.h"

#include "StateTreeExecutionContext.h"
#include "TowerOffenseGameMode.h"
#include "Kismet/GameplayStatics.h"

EStateTreeRunStatus FSTTask_EnsureEnemiesExists::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!IsValid(InstanceData.AIController)) return EStateTreeRunStatus::Failed;

	const UWorld* World = Context.GetWorld();
	if (!IsValid(World)) return EStateTreeRunStatus::Failed;

	ATowerOffenseGameMode* TowerOffenseGameMode = Cast<ATowerOffenseGameMode>(UGameplayStatics::GetGameMode(World));
	if (!IsValid(TowerOffenseGameMode)) return EStateTreeRunStatus::Failed;

	TArray<AActor*> Enemies{TowerOffenseGameMode->GetPlayers()};
	InstanceData.Enemies.Array = Enemies;

	return EStateTreeRunStatus::Succeeded;
}
