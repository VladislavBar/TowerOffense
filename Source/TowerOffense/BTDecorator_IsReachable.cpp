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

	if (FVector::DistXY(TowerPawn->GetActorLocation(), Target->GetActorLocation()) > MaxDistance) return false;
	if (HasStraightView(TowerPawn, Target)) return true;

	return false;
}

bool UBTDecorator_IsReachable::HasStraightView(const ATowerPawn* TowerPawn, const ATankPawn* Target) const
{
	if(!IsValid(TowerPawn) || !IsValid(Target)) return false;
	
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return false;

	FRotator ExpectedRotation = UKismetMathLibrary::FindLookAtRotation(TowerPawn->GetActorLocation(), Target->GetActorLocation());
	const FRotator CurrentRotation = TowerPawn->GetRelativeTurretMeshRotation();
	const FVector ExpectedProjectileSpawnLocation = TowerPawn->GetActorLocation() + (ExpectedRotation - CurrentRotation).RotateVector(TowerPawn->GetRelativeProjectileSpawnLocation());

	FHitResult HitResult;
	UKismetSystemLibrary::LineTraceSingle(World, ExpectedProjectileSpawnLocation, Target->GetActorLocation(), UEngineTypes::ConvertToTraceType(ECC_Visibility), false, TArray<AActor*>(), EDrawDebugTrace::None, HitResult, true, FLinearColor::Green, FLinearColor::Red, 1.f);
	return HitResult.GetActor() == Target;
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