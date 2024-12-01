#pragma once

#include "Components/CapsuleComponent.h"
#include "CoreMinimal.h"
#include "HealthComponent.h"
#include "Projectile.h"
#include "GameFramework/Pawn.h"

#include "TurretPawn.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTurretPawn, Log, All);

class UHealthBarWidgetComponent;
class ATankPawn;

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

UENUM()
enum class ETeam : uint8
{
	None,
	Team1,
	Team2,
	Team3,
	Team4,
	Towers,
	MAX
};

class ATurretPawn;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnActorTickableEnabledDelegate, bool);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnTeamChangedDelegate, const ATurretPawn*, ETeam);

UCLASS()
class TOWEROFFENSE_API ATurretPawn : public APawn
{
	GENERATED_BODY()

	friend class ATowerOffenseGameMode;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UCapsuleComponent> NewRootComponent;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentTeam, EditAnywhere, Category = "Turret|Team")
	ETeam CurrentTeam = ETeam::Team1;

	UPROPERTY(EditDefaultsOnly, Category = "Turret|Team")
	TObjectPtr<UDataTable> TeamColorTable;

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

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UHealthBarWidgetComponent> HealthBarWidgetComponent;

	FOnTeamChangedDelegate OnTeamChangedDelegate;

	UPROPERTY(ReplicatedUsing = OnRep_bIsInitialized)
	bool bIsInitialized = false;

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> BaseMesh;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> TurretMesh;

	UPROPERTY()
	UMaterialInstanceDynamic* TeamColorDynamicMaterial;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USceneComponent> ProjectileSpawnPoint;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UHealthComponent> HealthComponent;

	bool bLockTarget = false;
	FTimerHandle FireCooldownTimerHandle;

private:
	FOnActorTickableEnabledDelegate OnActorTickableEnabledDelegate;

	UPROPERTY(EditAnywhere, meta = (GetOptions = "GetMaterialTeamColorSlotNames"))
	FName MaterialTeamColorSlotName;

	UPROPERTY(EditAnywhere)
	FName MaterialTeamColorParameterName;

	UPROPERTY()
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
	float GetTurretMeshRotationYaw() const { return GetTurretMeshRotation().Yaw; }
	FVector GetProjectileSpawnLocation() const;
	float GetProjectileSpeed() const { return ProjectileSpeed; }

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientFire();

	void RotateTurretMeshToLocation(const float DeltaSeconds, const FVector& Location, bool bInstantRotation = false);
	FDelegateHandle AddOnActorTickableEnabledHandler(const FOnActorTickableEnabledDelegate::FDelegate& Delegate);
	void RemoveOnActorTickableEnabledHandler(const FDelegateHandle& Handle);

	FDelegateHandle AddOnTeamChangedHandler(const FOnTeamChangedDelegate::FDelegate& Delegate);
	void RemoveOnTeamChangedHandler(const FDelegateHandle& Handle);
	void UpdateMaterialTeamColor();

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

	UFUNCTION()
	void OnRep_CurrentTeam();

	UFUNCTION(Server, Reliable)
	void ServerFinishInitialization();

public:
	void TakeHit(float DamageAmount);
	FVector GetRelativeProjectileSpawnLocation() const;
	FRotator GetRelativeTurretMeshRotation() const;

	bool HasStraightView(const AActor* Target) const;
	bool CanReach(const AActor* Target, const float MaxDistance) const;

protected:
	friend class UBT_IdleRotation;
	friend struct FSTTask_EnemyTowerIdleRotation;
	friend class UBTTask_RotateEnemyTurretToTarget;
	friend struct FSTTask_RotateToTarget;

	void RotateTurretByYaw(const float Yaw) const;
	FVector PredictTargetLocation(const ATankPawn* Target, float StartPredictingLocationAtAccelerationProgress) const;

	void SetTargetLocation(const FVector& Location);
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;

	virtual bool CanFire() const;
	virtual void OnSuccessfulFire() const;

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlayVFXOnFire() const;

	UFUNCTION(Client, Reliable)
	virtual void ClientPlayVFXOnFire() const;
	virtual void SetActorTickEnabled(bool bEnabled) override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetActorTickEnabled(const bool bEnabled);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Exec)
	void CommandLineSetTeam(const ETeam NewTeam);

	UFUNCTION(Server, Reliable)
	void ServerSetTeam(const ETeam NewTeam);
	void SetTeam(const ETeam NewTeam);

	UFUNCTION(Server, Reliable)
	void SyncPlayerStateTeam() const;

	UFUNCTION()
	void OnRep_bIsInitialized();

	UFUNCTION(Server, Reliable)
	void ServerSyncUserState();

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

	UFUNCTION(NetMulticast, Reliable)
	void MulticastEmitOnDeathEffect() const;
	void EmitOnDeathSFX() const;
	void EmitOnDeathVFX() const;
	void EmitOnDeathCameraShake() const;

	void EnableRotationSound();
	void AdjustRotationSoundVolume(const float RotationDifference);

	void RefreshHealthBarVisibility() const;
	bool IsVisibleToPlayer() const;
	FColor GetTeamColor() const;

public:
	ATurretPawn();

	ETeam GetTeam() const { return CurrentTeam; }
};
