#pragma once

#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "CoreMinimal.h"
#include "TankPawn.h"
#include "TowerPawn.h"

#include "BTDecorator_IsReachable.generated.h"

UCLASS()
class TOWEROFFENSE_API UBTDecorator_IsReachable : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()

	UPROPERTY(EditInstanceOnly)
	float MaxDistance = 200.f;

	UPROPERTY(EditInstanceOnly)
	FBlackboardKeySelector TargetKey;

public:
	UBTDecorator_IsReachable();

private:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	bool HasStraightView(const ATowerPawn* OwnerComp, const ATankPawn* Target) const;
	ATowerPawn* GetTowerPawn(const UBehaviorTreeComponent& OwnerComp) const;
	virtual FString GetStaticDescription() const override;
};
