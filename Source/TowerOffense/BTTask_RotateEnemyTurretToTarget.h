#pragma once

#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "CoreMinimal.h"

#include "BTTask_RotateEnemyTurretToTarget.generated.h"

UCLASS()
class TOWEROFFENSE_API UBTTask_RotateEnemyTurretToTarget : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	UBTTask_RotateEnemyTurretToTarget();

	FRotator PreviousSelfRotation;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual FString GetStaticDescription() const override;
};
