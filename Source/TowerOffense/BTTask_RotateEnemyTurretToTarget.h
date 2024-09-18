#pragma once

#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "CoreMinimal.h"

#include "BTTask_RotateEnemyTurretToTarget.generated.h"

UCLASS()
class TOWEROFFENSE_API UBTTask_RotateEnemyTurretToTarget : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	UBTTask_RotateEnemyTurretToTarget();

private:
	UPROPERTY(EditAnywhere)
	float RotationTolerance = 0.1f;
	FRotator PreviousSelfRotation;

private:
	virtual FString GetStaticDescription() const override;
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
