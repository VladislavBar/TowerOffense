#pragma once

#include "Components/CapsuleComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "TurretPawn.generated.h"

UCLASS()
class TOWEROFFENSE_API ATurretPawn : public APawn
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TObjectPtr<UCapsuleComponent> NewRootComponent;

	// The maximum speed at which the turret can rotate when it should almost instantly rotate to the target location (incl. multipliers)
	UPROPERTY(EditAnywhere)
	int MaxInstantRotationSpeed = 10;

	UPROPERTY(EditAnywhere)
	float RotationSpeedWhenTargetLocked = 10;

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> BaseMesh;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> TurretMesh;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USceneComponent> ProjectileSpawnPoint;

	bool bLockTarget = false;

private:
	UPROPERTY(EditAnywhere, meta = (GetOptions = "GetMaterialTeamColorSlotNames"))
	FName MaterialTeamColorSlotName;

	UPROPERTY(EditAnywhere)
	FName MaterialTeamColorParameterName;

	UPROPERTY(EditAnywhere)
	FColor TeamColor;

	UFUNCTION()
	TArray<FName> GetMaterialTeamColorSlotNames() const;

	FVector TargetLocation = FVector::ZeroVector;

protected:
	UPROPERTY(EditAnywhere, Category = "Turret Rotation", meta = (ClampMin = "0.0"))
	float RotationInterpExponent = 2.f;
	float MeshDefaultRotationYaw = 90.f;

public:
	UFUNCTION(BlueprintCallable)
	void Fire();
	void RotateTurretMeshToLocation(const float DeltaSeconds, const FVector& Location, bool bInstantRotation = false);
	FRotator GetTurretMeshRotation() const;
	FVector GetProjectileSpawnLocation() const;

protected:
	void SetTargetLocation(const FVector& Location);
	virtual void Tick(float DeltaSeconds) override;

private:
	void SetupTeamColorDynamicMaterial(UStaticMeshComponent* Mesh);
	virtual void PostInitializeComponents() override;
	void RotateTurretMesh(const float DeltaSeconds);

public:
	ATurretPawn();
};
