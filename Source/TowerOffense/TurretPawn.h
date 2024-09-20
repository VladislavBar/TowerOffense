#pragma once

#include "Components/CapsuleComponent.h"
#include "CoreMinimal.h"
#include "Projectile.h"
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
	int32 MaxInstantRotationSpeed = 10;

	UPROPERTY(EditAnywhere)
	float RotationSpeedWhenTargetLocked = 10.f;

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> BaseMesh;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> TurretMesh;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USceneComponent> ProjectileSpawnPoint;

	bool bLockTarget = false;
	FTimerHandle FireCooldownTimerHandle;

private:
	UPROPERTY(EditAnywhere, meta = (GetOptions = "GetMaterialTeamColorSlotNames"))
	FName MaterialTeamColorSlotName;

	UPROPERTY(EditAnywhere)
	FName MaterialTeamColorParameterName;

	UPROPERTY(EditAnywhere)
	FColor TeamColor;

	UFUNCTION()
	TArray<FName> GetMaterialTeamColorSlotNames() const;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AProjectile> ProjectileClass;

	FVector TargetLocation = FVector::ZeroVector;
	float ProjectileDebugSphereRadius = 10.f;
	int32 ProjectileDebugSphereSegments = 12;
	FColor ProjectileDebugSphereColor = FColor::Red;

	bool bCanFire = true;

	UPROPERTY(EditAnywhere, Category = "Turret|Fire", meta = (ClampMin = "0.0"))
	float FireCooldown = 5.f;

	UPROPERTY(EditAnywhere, Category = "Turret|Fire", meta = (ClampMin = "0.0"))
	float ProjectileSpeed = 7000.f;

protected:
	UPROPERTY(EditAnywhere, Category = "Turret Rotation", meta = (ClampMin = "0.0"))
	float RotationInterpExponent = 2.f;

public:
	UFUNCTION(BlueprintCallable)
	void Fire();
	void RotateTurretMeshToLocation(const float DeltaSeconds, const FVector& Location, bool bInstantRotation = false);
	FRotator GetTurretMeshRotation() const;
	FVector GetProjectileSpawnLocation() const;
	float GetProjectileSpeed() const { return ProjectileSpeed; }

protected:
	void SetTargetLocation(const FVector& Location);
	virtual void Tick(float DeltaSeconds) override;

private:
	void SetupTeamColorDynamicMaterial(UStaticMeshComponent* Mesh);
	virtual void PostInitializeComponents() override;
	void RotateTurretMesh(const float DeltaSeconds);
	void DrawDebugAtSpawnPointLocation() const;
	void RotateWithoutInterp(const FVector& CurrentTargetLocation, const float DeltaSeconds);
	void RotateWithInterp(const FVector& CurrentTargetLocation, const float DeltaSeconds);
	void SetSpawnPointRotationAtLocation(const FVector& CurrentTargetLocation);
	void StartCooldownTimer();
	void DisableFire();
	void EnableFire();

public:
	ATurretPawn();
};
