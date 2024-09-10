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
	SelfTurret->RotateTurretMeshToLocation(DeltaSeconds, Target->GetActorLocation(), true);

	if (PreviousSelfRotation.Equals(SelfTurret->GetTurretMeshRotation(), 0.1f))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

FString UBTTask_RotateEnemyTurretToTarget::GetStaticDescription() const
{
	return FString::Printf(TEXT("Rotate this EnemyTurret to the TargetLocation"));
}