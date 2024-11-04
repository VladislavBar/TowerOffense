#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ActorArrayWrapper.h"
#include "StateTreeTaskBase.h"
#include "UObject/NoExportTypes.h"
#include "Conditions/StateTreeCommonConditions.h"
#include "STTask_EnsureEnemiesExist.generated.h"

enum class EStateTreeRunStatus : uint8;
struct FStateTreeTransitionResult;

USTRUCT()
struct FStateTreeEnsureEnemiesExistsTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AAIController> AIController = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Output")
	FActorArrayWrapper Enemies;
};

USTRUCT(meta = (DisplayName = "Enemy Tower Ensure Enemies Exists Task", Category = "AI|EnemyTower"))
struct FSTTask_EnsureEnemiesExists : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeEnsureEnemiesExistsTaskInstanceData;

	FSTTask_EnsureEnemiesExists() = default;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};
