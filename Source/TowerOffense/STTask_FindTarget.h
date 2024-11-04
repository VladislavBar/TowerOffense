#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ActorArrayWrapper.h"
#include "TurretPawn.h"
#include "Tasks/StateTreeAITask.h"
#include "StateTreeModule/Public/StateTreeEvaluatorBase.h"
#include "STTask_FindTarget.generated.h"

enum class EStateTreeRunStatus : uint8;
struct FStateTreeTransitionResult;

USTRUCT()
struct FStateTreeFindTargetTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = Context)
	FActorArrayWrapper Enemies;

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AAIController> AIController;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "180.0"))
	float SearchHalfAngle = 45.f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"))
	float SearchDistance = 1000.f;

	UPROPERTY(EditAnywhere)
	bool bCheckOnEntryOnly = false;

	UPROPERTY(EditAnywhere)
	bool bFinishTaskOnFirstTarget = false;

	UPROPERTY(EditAnywhere, Category = Output)
	AActor* Target;
};

// TODO: Implement the same thing but using StateTree_EnterCondition that will allow us to get rid of targeting state's tasks
USTRUCT(meta = (DisplayName = "Enemy Tower Find Target", Category = "AI|EnemyTower"))
struct FStateTreeFindTargetsTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeFindTargetTaskInstanceData;

	FStateTreeFindTargetsTask() = default;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	static AActor* FindEnemy(const FStateTreeFindTargetTaskInstanceData& InstanceData);
	EStateTreeRunStatus ExecuteTask(const FStateTreeExecutionContext& Context) const;
};
