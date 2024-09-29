#include "BTDecorator_IsOwnerTickable.h"

#include "AIController.h"

bool UBTDecorator_IsOwnerTickable::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if (!IsValid(&OwnerComp)) return false;

	const AAIController* Owner = OwnerComp.GetAIOwner();
	if (!IsValid(Owner)) return false;

	const APawn* PossessedPawn = Owner->GetPawn();
	if (!IsValid(PossessedPawn)) return false;

	return PossessedPawn->IsActorTickEnabled();
}

FString UBTDecorator_IsOwnerTickable::GetStaticDescription() const
{
	return FString::Printf(TEXT("Check if the owner of the behavior tree can tick."));
}