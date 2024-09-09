#include "BTDecorator_IsReachable.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TankPawn.h"

UBTDecorator_IsReachable::UBTDecorator_IsReachable()
{
	TargetLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_IsReachable, TargetLocationKey));
	NodeName = TEXT("Is Reachable (From Enemy Tower to Tank Pawn)");
}

bool UBTDecorator_IsReachable::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if (!IsValid(&OwnerComp)) return false;

	const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!IsValid(Blackboard)) return false;

	const FVector TargetLocation = Blackboard->GetValueAsVector(TargetLocationKey.SelectedKeyName);

	const ATowerPawn* TowerPawn = GetTowerPawn(OwnerComp);
	if (!IsValid(TowerPawn)) return false;

	if (FVector::DistXY(TowerPawn->GetActorLocation(), TargetLocation) > MaxDistance) return false;
	if (HasStraightView(TowerPawn, TargetLocation)) return true;

	return false;
}

bool UBTDecorator_IsReachable::HasStraightView(const ATowerPawn* TowerPawn, const FVector& TargetLocation) const
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return false;

	const ATankPawn* TankPawn = GetPlayerTankPawn(World);
	if (!IsValid(TankPawn)) return false;

	FRotator ExpectedRotation = UKismetMathLibrary::FindLookAtRotation(TowerPawn->GetActorLocation(), TargetLocation);
	TowerPawn->AdjustRotationToMeshRotationOffset(ExpectedRotation);
	const FRotator CurrentRotation = TowerPawn->GetRelativeTurretMeshRotation();
	const FVector ExpectedProjectileSpawnLocation = TowerPawn->GetActorLocation() + (ExpectedRotation - CurrentRotation).RotateVector(TowerPawn->GetRelativeProjectileSpawnLocation());

	FHitResult HitResult;
	UKismetSystemLibrary::LineTraceSingle(World, ExpectedProjectileSpawnLocation, TargetLocation, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, TArray<AActor*>(), EDrawDebugTrace::None, HitResult, true, FLinearColor::Green, FLinearColor::Red, 1.f);
	return HitResult.GetActor() == TankPawn;
}

ATowerPawn* UBTDecorator_IsReachable::GetTowerPawn(const UBehaviorTreeComponent& OwnerComp) const
{
	const AAIController* AIController = OwnerComp.GetAIOwner();
	if (!IsValid(AIController)) return nullptr;

	return Cast<ATowerPawn>(AIController->GetPawn());
}

ATankPawn* UBTDecorator_IsReachable::GetPlayerTankPawn(const UWorld* World) const
{
	const APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!IsValid(PlayerController)) return nullptr;

	return Cast<ATankPawn>(PlayerController->GetPawn());
}

FString UBTDecorator_IsReachable::GetStaticDescription() const
{
	return FString::Printf(TEXT("Check if the tower has a straight view to the tank pawn within %f units."), MaxDistance);
}