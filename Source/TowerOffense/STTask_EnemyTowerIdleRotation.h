#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "StateTreeTaskBase.h"
#include "STTask_EnemyTowerIdleRotation.generated.h"

enum class EStateTreeRunStatus : uint8;
struct FStateTreeTransitionResult;

USTRUCT()
struct FStateTreeIdleRotationTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AAIController> AIController = nullptr;

	UPROPERTY(EditAnywhere)
	float RotationSpeed = 90.f;

	UPROPERTY(EditAnywhere)
	bool bClockwiseRotation = true;
};

USTRUCT(meta = (DisplayName = "Enemy Tower Idle Rotation Task", Category = "AI|EnemyTower"))
struct FSTTask_EnemyTowerIdleRotation : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeIdleRotationTaskInstanceData;

	FSTTask_EnemyTowerIdleRotation() = default;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
};