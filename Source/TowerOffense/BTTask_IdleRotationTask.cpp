#include "BTTask_IdleRotationTask.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "TowerPawn.h"
#include "TurretPawn.h"

UBT_IdleRotation::UBT_IdleRotation()
{
	bNotifyTick = true;
	NodeName = TEXT("Idle EnemyTower Rotation");
}

EBTNodeResult::Type UBT_IdleRotation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::InProgress;
}

void UBT_IdleRotation::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	const AAIController* Controller = OwnerComp.GetAIOwner();
	if (!IsValid(Controller)) return FinishLatentAbort(OwnerComp);

	ATowerPawn* SelfTurret = Cast<ATowerPawn>(Controller->GetPawn());
	if (!IsValid(SelfTurret)) return FinishLatentAbort(OwnerComp);

	SelfTurret->RotateByYaw((bClockwiseRotation ? 1 : -1) * RotationSpeed * DeltaSeconds);
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}

FString UBT_IdleRotation::GetStaticDescription() const
{
	return FString::Printf(TEXT("Rotates the turret by %f degrees per second."), RotationSpeed);
}