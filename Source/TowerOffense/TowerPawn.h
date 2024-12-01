#pragma once

#include "CoreMinimal.h"
#include "HealthBarWidgetComponent.h"
#include "TurretPawn.h"

#include "TowerPawn.generated.h"

UCLASS()
class TOWEROFFENSE_API ATowerPawn : public ATurretPawn
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	bool bRotateClockwise = true;

public:
	ATowerPawn();

private:
	virtual void BeginPlay() override;
};
