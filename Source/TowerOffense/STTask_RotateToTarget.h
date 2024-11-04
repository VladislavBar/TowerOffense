#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "StateTreeTaskBase.h"
#include "UObject/NoExportTypes.h"
#include "STTask_RotateToTarget.generated.h"

enum class EStateTreeRunStatus : uint8;
struct FStateTreeTransitionResult;

USTRUCT()
struct FStateTreeRotateToTargetInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AAIController> AIController = nullptr;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<AActor> Target = nullptr;

	UPROPERTY(EditAnywhere)
	float RotationTolerance = 1.f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float StartPredictingLocationAtAccelerationProgress = 0.6f;
	FRotator PreviousSelfRotation;
	FVector PredictedLocation;
};

USTRUCT(meta = (DisplayName = "Enemy Tower Rotation To Target Task", Category = "AI|EnemyTower"))
struct FSTTask_RotateToTarget : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeRotateToTargetInstanceData;

	FSTTask_RotateToTarget() = default;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
};