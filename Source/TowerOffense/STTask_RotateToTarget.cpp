#include "STTask_RotateToTarget.h"

#include "StateTreeExecutionContext.h"
#include "TankPawn.h"
#include "TurretPawn.h"

EStateTreeRunStatus FSTTask_RotateToTarget::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	Super::Tick(Context, DeltaTime);

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!IsValid(InstanceData.AIController)) return EStateTreeRunStatus::Failed;

	const AAIController* Controller = InstanceData.AIController;
	if (!IsValid(Controller)) return EStateTreeRunStatus::Failed;

	ATurretPawn* SelfTurret = Cast<ATurretPawn>(Controller->GetPawn());
	if (!IsValid(SelfTurret)) return EStateTreeRunStatus::Failed;

	const ATankPawn* Target = Cast<ATankPawn>(InstanceData.Target);
	if (!IsValid(Target)) return EStateTreeRunStatus::Failed;

	if (Target->IsPendingKillPending()) return EStateTreeRunStatus::Succeeded;

	InstanceData.PreviousSelfRotation = SelfTurret->GetTurretMeshRotation();
	const FVector PredictedTargetLocation = SelfTurret->PredictTargetLocation(Target, InstanceData.StartPredictingLocationAtAccelerationProgress);
	SelfTurret->RotateTurretMeshToLocation(DeltaTime, PredictedTargetLocation, true);

	return EStateTreeRunStatus::Running;
}