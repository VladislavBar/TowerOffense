#include "STTask_FindTarget.h"

#include "StateTreeExecutionContext.h"
#include "TankPawn.h"
#include "TurretPawn.h"
#include "Kismet/KismetMathLibrary.h"

EStateTreeRunStatus FStateTreeFindTargetsTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	return ExecuteTask(Context);
}

EStateTreeRunStatus FStateTreeFindTargetsTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.bCheckOnEntryOnly && IsValid(InstanceData.Target)) return EStateTreeRunStatus::Running;
	if (InstanceData.bCheckOnEntryOnly && !IsValid(InstanceData.Target)) return EStateTreeRunStatus::Failed;
	
	return ExecuteTask(Context);
}

AActor* FStateTreeFindTargetsTask::FindEnemy(const FStateTreeFindTargetTaskInstanceData& InstanceData)
{
	if (InstanceData.SearchDistance <= 0.f || InstanceData.SearchHalfAngle <= 0 || InstanceData.Enemies.Array.Num() == 0 || !InstanceData.AIController)
	{
		return nullptr;
	}

	const ATurretPawn* SelfActor = Cast<ATurretPawn>(InstanceData.AIController->GetPawn());
	if (!SelfActor) return nullptr;
	
	for (AActor* Enemy : InstanceData.Enemies.Array)
	{
		if (!IsValid(Enemy)) continue;

		const FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(SelfActor->GetActorLocation(), Enemy->GetActorLocation());
		const float DeltaRotationYaw = FRotator::NormalizeAxis(TargetRotation.Yaw - SelfActor->GetTurretMeshRotation().Yaw);

		if (InstanceData.SearchHalfAngle - UKismetMathLibrary::Abs(DeltaRotationYaw) <= 0.f) continue;
		if (!SelfActor->CanReach(Enemy, InstanceData.SearchDistance)) continue;

		return Enemy;
	}

	return nullptr;
}

EStateTreeRunStatus FStateTreeFindTargetsTask::ExecuteTask(const FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!IsValid(InstanceData.AIController)) return EStateTreeRunStatus::Failed;

	InstanceData.Target = FindEnemy(InstanceData);
	if (IsValid(InstanceData.Target)) return EStateTreeRunStatus::Succeeded;
	return EStateTreeRunStatus::Running;
}