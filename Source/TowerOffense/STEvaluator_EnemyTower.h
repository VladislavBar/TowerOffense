#pragma once

#include "Conditions/StateTreeCommonConditions.h"
#include "CoreMinimal.h"
#include "AIController.h"
#include "ActorArrayWrapper.h"
#include "StateTreeEvaluatorBase.h"
#include "STEvaluator_EnemyTower.generated.h"

USTRUCT()
struct TOWEROFFENSE_API FSTEvaluatorInstanceData_EnemyTower
{
	GENERATED_BODY()

	friend struct FSTEvaluator_EnemyTower;

	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> Actor = nullptr;

	UPROPERTY(EditAnywhere, Category = "Output")
	TObjectPtr<AAIController> AIController = nullptr;
};

USTRUCT(DisplayName = "Enemy Tower Evaluator", Category = "EnemyTower")
struct TOWEROFFENSE_API FSTEvaluator_EnemyTower : public FStateTreeEvaluatorCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTEvaluatorInstanceData_EnemyTower;

	FSTEvaluator_EnemyTower() = default;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual void TreeStart(FStateTreeExecutionContext& Context) const override;
};
