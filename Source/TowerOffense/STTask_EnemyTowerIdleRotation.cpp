#include "STTask_EnemyTowerIdleRotation.h"

#include "StateTreeExecutionContext.h"
#include "TowerPawn.h"

EStateTreeRunStatus FSTTask_EnemyTowerIdleRotation::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!IsValid(InstanceData.AIController)) return EStateTreeRunStatus::Failed;

	ATowerPawn* SelfTurret = Cast<ATowerPawn>(InstanceData.AIController->GetPawn());
	if (!IsValid(SelfTurret)) return EStateTreeRunStatus::Failed;

	SelfTurret->RotateTurretByYaw((InstanceData.bClockwiseRotation ? 1.f : -1.f) * InstanceData.RotationSpeed * DeltaTime);
	return EStateTreeRunStatus::Running;
}