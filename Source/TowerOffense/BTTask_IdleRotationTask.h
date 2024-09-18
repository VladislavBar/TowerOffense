#pragma once

#include "BehaviorTree/BTTaskNode.h"
#include "CoreMinimal.h"

#include "BTTask_IdleRotationTask.generated.h"

UCLASS()
class TOWEROFFENSE_API UBT_IdleRotation : public UBTTaskNode
{
	GENERATED_BODY()

	UPROPERTY(EditInstanceOnly)
	float RotationSpeed = 90.f;

	UPROPERTY(EditInstanceOnly)
	bool bClockwiseRotation = true;

public:
	UBT_IdleRotation();

private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual FString GetStaticDescription() const override;
};
