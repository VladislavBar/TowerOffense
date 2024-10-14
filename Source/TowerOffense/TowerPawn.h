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

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UHealthBarWidgetComponent> HealthBarWidgetComponent;

public:
	ATowerPawn();

	FVector GetRelativeProjectileSpawnLocation() const;
	FRotator GetRelativeTurretMeshRotation() const;

	void RotateByYaw(const float Yaw);

private:
	void RefreshHealthBarVisibility();
	bool IsVisibleToPlayer() const;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
