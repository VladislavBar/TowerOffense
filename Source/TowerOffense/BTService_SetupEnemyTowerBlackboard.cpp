#include "BTService_SetupEnemyTowerSetupBlackboard.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "TowerPawn.h"

UBTService_SetupEnemyTowerBlackboard::UBTService_SetupEnemyTowerBlackboard()
{
	bNotifyOnSearch = true;
	NodeName = TEXT("Setup Enemy Tower Blackboard");
}

void UBTService_SetupEnemyTowerBlackboard::OnSearchStart(FBehaviorTreeSearchData& SearchData)
{
	Super::OnSearchStart(SearchData);

	if (!IsValid(&SearchData.OwnerComp)) return;

	UBlackboardComponent* Blackboard = SearchData.OwnerComp.GetBlackboardComponent();
	if (!IsValid(Blackboard)) return;

	const AAIController* Controller = SearchData.OwnerComp.GetAIOwner();
	if (!IsValid(Controller)) return;

	const ATowerPawn* SelfTurret = Cast<ATowerPawn>(Controller->GetPawn());
	if (!IsValid(SelfTurret)) return;

	const AActor* PlayerPawn = GetPlayerPawn();
	if (!IsValid(PlayerPawn)) return;

	Blackboard->SetValueAsVector("SelfTurretForwardVector", SelfTurret->GetActorLocation() + SelfTurret->GetTurretMeshRotation().Vector());
	Blackboard->SetValueAsVector("TargetLocation", PlayerPawn->GetActorLocation());
}

AActor* UBTService_SetupEnemyTowerBlackboard::GetPlayerPawn() const
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return nullptr;

	const APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!IsValid(PlayerController)) return nullptr;

	return PlayerController->GetPawn();
}

FString UBTService_SetupEnemyTowerBlackboard::GetStaticDescription() const
{
	return FString::Printf(TEXT("Sets up SelfTurretForwardVector and TargetLocation in Blackboard"));
}