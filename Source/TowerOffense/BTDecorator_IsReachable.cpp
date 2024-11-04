#include "BTDecorator_IsReachable.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TankPawn.h"

UBTDecorator_IsReachable::UBTDecorator_IsReachable()
{
	TargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_IsReachable, TargetKey), ATankPawn::StaticClass());
	NodeName = TEXT("Is Reachable (From Enemy Tower to Tank Pawn)");
}

bool UBTDecorator_IsReachable::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if (!IsValid(&OwnerComp)) return false;

	const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!IsValid(Blackboard)) return false;

	const ATankPawn* Target = Cast<ATankPawn>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));
	if(!IsValid(Target)) return false;
	
	const ATowerPawn* TowerPawn = GetTowerPawn(OwnerComp);
	if (!IsValid(TowerPawn)) return false;

	return TowerPawn->CanReach(Target, MaxDistance);
}

ATowerPawn* UBTDecorator_IsReachable::GetTowerPawn(const UBehaviorTreeComponent& OwnerComp) const
{
	const AAIController* AIController = OwnerComp.GetAIOwner();
	if (!IsValid(AIController)) return nullptr;

	return Cast<ATowerPawn>(AIController->GetPawn());
}

FString UBTDecorator_IsReachable::GetStaticDescription() const
{
	return FString::Printf(TEXT("Check if the tower has a straight view to the tank pawn within %f units."), MaxDistance);
}