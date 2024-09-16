#pragma once

#include "CoreMinimal.h"
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
	void RotateByYaw(const float Yaw);
	FVector GetRelativeProjectileSpawnLocation() const;
	FRotator GetRelativeTurretMeshRotation() const;
};
