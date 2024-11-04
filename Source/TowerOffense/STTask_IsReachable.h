#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "StateTreeTaskBase.h"
#include "UObject/NoExportTypes.h"
#include "STTask_IsReachable.generated.h"

enum class EStateTreeRunStatus : uint8;
struct FStateTreeTransitionResult;

USTRUCT()
struct FStateTreeIsReachableInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AAIController> AIController = nullptr;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<AActor> Target = nullptr;

	UPROPERTY(EditAnywhere, Category = Input)
	float VisibilityDistance = 1000.f;
};

USTRUCT(meta = (DisplayName = "Enemy Tower Is Still Reachable Task", Category = "AI|EnemyTower"))
struct FSTTask_IsReachable : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeIsReachableInstanceData;

	FSTTask_IsReachable() = default;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	EStateTreeRunStatus ExecuteTask(const FStateTreeExecutionContext& Context) const;
};
