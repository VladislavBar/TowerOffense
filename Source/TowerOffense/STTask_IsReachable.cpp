#include "STTask_IsReachable.h"

#include "StateTreeExecutionContext.h"
#include "TurretPawn.h"

EStateTreeRunStatus FSTTask_IsReachable::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	return ExecuteTask(Context);
}
EStateTreeRunStatus FSTTask_IsReachable::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	return ExecuteTask(Context);
}

EStateTreeRunStatus FSTTask_IsReachable::ExecuteTask(const FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!IsValid(InstanceData.AIController) || !IsValid(InstanceData.Target)) return EStateTreeRunStatus::Failed;

	const ATurretPawn* SelfActor = Cast<ATurretPawn>(InstanceData.AIController->GetPawn());
	if (!SelfActor) return EStateTreeRunStatus::Failed;

	const bool bIsReachable = SelfActor->CanReach(InstanceData.Target, InstanceData.VisibilityDistance);
	return bIsReachable ? EStateTreeRunStatus::Running : EStateTreeRunStatus::Failed;
}