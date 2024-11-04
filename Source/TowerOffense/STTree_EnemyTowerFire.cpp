#include "STTree_EnemyTowerFire.h"

#include "StateTreeExecutionContext.h"
#include "TurretPawn.h"

EStateTreeRunStatus FSTTree_EnemyTowerFire::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FSTTree_EnemyTowerFire::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!IsValid(InstanceData.AIController)) return EStateTreeRunStatus::Failed;

	ATurretPawn* SelfTurret = Cast<ATurretPawn>(InstanceData.AIController->GetPawn());
	if (!IsValid(SelfTurret)) return EStateTreeRunStatus::Failed;

	SelfTurret->ClientFire();
	return EStateTreeRunStatus::Running;
}