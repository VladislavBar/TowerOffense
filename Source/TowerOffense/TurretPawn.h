#pragma once

#include "Components/CapsuleComponent.h"
#include "CoreMinimal.h"
#include "HealthComponent.h"
#include "Projectile.h"
#include "GameFramework/Pawn.h"

#include "TurretPawn.generated.h"

USTRUCT()
struct FCameraShakeData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Camera Shake")
	TSubclassOf<UCameraShakeBase> CameraShakeClass;

	UPROPERTY(EditAnywhere, Category = "Camera Shake")
	float InnerRadius = 0.f;

	UPROPERTY(EditAnywhere, Category = "Camera Shake")
	float OuterRadius = 10.f;

	UPROPERTY(EditAnywhere, Category = "Camera Shake")
	float Falloff = 1.f;
};

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

	UPROPERTY(EditAnywhere, Category = "Turret|VFX")
	FCameraShakeData OnDeathCameraShakeData;

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

	UPROPERTY(ReplicatedUsing = OnRep_bCanFire)
	bool bCanFire = true;

	UPROPERTY(EditAnywhere, Category = "Turret|Fire", meta = (ClampMin = "0.0"))
	float FireCooldown = 5.f;

	UPROPERTY(EditAnywhere, Category = "Turret|Fire", meta = (ClampMin = "0.0"))
	float ProjectileSpeed = 7000.f;

	FOnDeathDelegate::FDelegate OnDeathDelegate;
	FDelegateHandle OnDeathDelegateHandle;

protected:
	UPROPERTY(EditAnywhere, Category = "Turret Rotation", meta = (ClampMin = "0.0"))
	float RotationInterpExponent = 2.f;

public:
	FRotator GetTurretMeshRotation() const;
	FVector GetProjectileSpawnLocation() const;
	float GetProjectileSpeed() const { return ProjectileSpeed; }

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientFire();

	void RotateTurretMeshToLocation(const float DeltaSeconds, const FVector& Location, bool bInstantRotation = false);

private:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();

	void RotateTurretMeshToLocation_Internal(const float DeltaSeconds, const FVector& Location, bool bInstantRotation = false);

	UFUNCTION(Server, Unreliable)
	void ServerRotateTurretMeshToLocation(const float DeltaSeconds, const FVector& Location, bool bInstantRotation = false);

	UFUNCTION(Client, Unreliable)
	void ClientRotateTurretMeshToLocation(const float DeltaSeconds, const FVector& Location, bool bInstantRotation = false);
	void PlaySoundOnRotation(const FRotator& PreviousRotation);

	UFUNCTION()
	void OnRep_bCanFire();

public:
	void TakeHit(float DamageAmount);

protected:
	void SetTargetLocation(const FVector& Location);
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;

	virtual bool CanFire() const;
	virtual void OnSuccessfulFire() const;

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlayVFXOnFire() const;

	UFUNCTION(Client, Reliable)
	virtual void ClientPlayVFXOnFire() const;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void SetupTeamColorDynamicMaterial(UStaticMeshComponent* Mesh);
	virtual void PostInitializeComponents() override;
	void RotateTurretMesh(const float DeltaSeconds);
	void DrawDebugAtSpawnPointLocation() const;
	void RotateWithoutInterp(const FVector& CurrentTargetLocation, const float DeltaSeconds);
	void RotateWithInterp(const FVector& CurrentTargetLocation, const float DeltaSeconds);
	void SetSpawnPointRotationAtLocation(const FVector& CurrentTargetLocation);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartCooldownTimer();

	void DisableFire();
	void EnableFire();

	void SetupOnDeathDelegate();
	void OnDeath();

	void EmitOnDeathEffect() const;
	void EmitOnDeathSFX() const;
	void EmitOnDeathVFX() const;
	void EmitOnDeathCameraShake() const;

	void EnableRotationSound();
	void AdjustRotationSoundVolume(const float RotationDifference);

public:
	ATurretPawn();
};
