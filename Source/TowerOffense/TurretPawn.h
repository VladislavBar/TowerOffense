#pragma once

#include "Components/CapsuleComponent.h"
#include "CoreMinimal.h"
#include "HealthComponent.h"
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

	UPROPERTY(EditAnywhere, Category = "Turret|Fire", meta = (ClampMin = "0.0"))
	float Damage = 10.f;

	UPROPERTY(EditAnywhere, Category = "Turret|VFX")
	TObjectPtr<UNiagaraComponent> OnFireEffectComponent;

	UPROPERTY(EditAnywhere, Category = "Turret|VFX")
	TObjectPtr<UParticleSystem> OnDeathEffect;

	UPROPERTY(EditAnywhere, Category = "Turret|SFX")
	TObjectPtr<USoundBase> OnDeathSound;

	UPROPERTY(EditAnywhere, Category = "Turret|SFX")
	TObjectPtr<UAudioComponent> OnRotationSoundComponent;

	UPROPERTY(EditAnywhere, Category = "Turret|SFX", meta = (ClampMin = "0.0"))
	float RotationSoundVolumeMultiplier = 0.1f;

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> BaseMesh;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> TurretMesh;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USceneComponent> ProjectileSpawnPoint;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UHealthComponent> HealthComponent;

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
	FRotator GetTurretMeshRotation() const;
	FVector GetProjectileSpawnLocation() const;
	float GetProjectileSpeed() const { return ProjectileSpeed; }

	UFUNCTION(BlueprintCallable)
	void Fire();
	void RotateTurretMeshToLocation(const float DeltaSeconds, const FVector& Location, bool bInstantRotation = false);
	void TakeHit(float DamageAmount);

protected:
	void SetTargetLocation(const FVector& Location);
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;

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

	void SetupOnDeathDelegate();
	void OnDeath();

	void EmitOnDeathEffect() const;
	void EmitOnDeathSFX() const;
	void EmitOnDeathVFX() const;

	void EnableRotationSound();
	void AdjustRotationSoundVolume(const float RotationDifference);
	void SetRotationSoundVolume(const float NewVolume);

public:
	ATurretPawn();
};
