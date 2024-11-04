#include "BTTask_RotateEnemyTurretToTarget.h"

#include "AIController.h"
#include "TankPawn.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "TurretPawn.h"

UBTTask_RotateEnemyTurretToTarget::UBTTask_RotateEnemyTurretToTarget()
{
	bNotifyTick = true;
	NodeName = "Rotate Enemy Turret To Target";
}

EBTNodeResult::Type UBTTask_RotateEnemyTurretToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::InProgress;
}

void UBTTask_RotateEnemyTurretToTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	const AAIController* Controller = OwnerComp.GetAIOwner();
	if (!IsValid(Controller)) return FinishLatentAbort(OwnerComp);

	ATurretPawn* SelfTurret = Cast<ATurretPawn>(Controller->GetPawn());
	if (!IsValid(SelfTurret)) return FinishLatentAbort(OwnerComp);

	const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!IsValid(Blackboard)) return FinishLatentAbort(OwnerComp);

	PreviousSelfRotation = SelfTurret->GetTurretMeshRotation();
	const ATankPawn* Target = Cast<ATankPawn>(Blackboard->GetValueAsObject("Target"));
	const FVector PredictedTargetLocation = SelfTurret->PredictTargetLocation(Target, StartPredictingLocationAtAccelerationProgress);
	SelfTurret->RotateTurretMeshToLocation(DeltaSeconds, PredictedTargetLocation, true);

	if (PreviousSelfRotation.Equals(SelfTurret->GetTurretMeshRotation(), RotationTolerance))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

FString UBTTask_RotateEnemyTurretToTarget::GetStaticDescription() const
{
	return FString::Printf(TEXT("Rotate this EnemyTurret to the TargetLocation with a tolerance of %f.\n Start predicting the TargetLocation at %f of the acceleration progress."), RotationTolerance, StartPredictingLocationAtAccelerationProgress);
}