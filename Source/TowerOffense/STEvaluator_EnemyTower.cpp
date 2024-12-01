#include "STEvaluator_EnemyTower.h"

#include "StateTreeExecutionContext.h"
#include "Kismet/GameplayStatics.h"

void FSTEvaluator_EnemyTower::TreeStart(FStateTreeExecutionContext& Context) const
{
	Super::TreeStart(Context);

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!IsValid(InstanceData.Actor)) return;

	InstanceData.AIController = Cast<AAIController>(InstanceData.Actor);
}