#include "BTTask_EnemyTowerFire.h"

#include "AIController.h"
#include "TurretPawn.h"

EBTNodeResult::Type UBTTask_EnemyTowerFire::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!IsValid(&OwnerComp)) return EBTNodeResult::Failed;

	const AAIController* Controller = OwnerComp.GetAIOwner();
	if (!IsValid(Controller)) return EBTNodeResult::Failed;

	ATurretPawn* SelfTurret = Cast<ATurretPawn>(Controller->GetPawn());
	if (!IsValid(SelfTurret)) return EBTNodeResult::Failed;

	SelfTurret->Fire();
	return EBTNodeResult::Succeeded;
}

FString UBTTask_EnemyTowerFire::GetStaticDescription() const
{
	return FString::Printf(TEXT("Spawn a projectile from the turret. Calles Fire() on the turret."));
}