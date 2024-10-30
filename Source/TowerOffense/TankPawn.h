#pragma once

#include "Camera/CameraComponent.h"
#include "CoreMinimal.h"
#include "AmmoComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "TurretPawn.h"

#include "TankPawn.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCooldownTickDelegate, const float);

UCLASS()
class TOWEROFFENSE_API ATankPawn : public ATurretPawn
{
	GENERATED_BODY()

public:
	FOnCooldownTickDelegate OnCooldownTickDelegate;

private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(EditAnywhere, Category = "Tank|Camera")
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> MoveForwardAction;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> TurnRightAction;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> FireAction;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> RotateCameraAction;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> SetTargetAction;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> ToggleCursorAction;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputMappingContext> GameControlInputMappingContext;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputMappingContext> StartDelayMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	TObjectPtr<UNiagaraComponent> VehicleSmokeEffect;

	UPROPERTY(EditAnywhere, Category = "Turret|SFX")
	TObjectPtr<UAudioComponent> MovementSoundComponent;

	UPROPERTY(EditDefaultsOnly, Category = "SFX", meta = (ClampMin = "0.0"))
	float MovementSoundVolumeMultiplier = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category = "SFX", meta = (ClampMin = "0.0"))
	float DefaultMovementSoundVolume = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category = "SFX", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MovementSoundReductionTime = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "SFX", meta = (ClampMin = "0.0"))
	float MovementSoundReductionExponent = 1.f;

	UPROPERTY(Replicated)
	float LastSoundVolume = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Tank Movement", meta = (ClampMin = "0.0"))
	float AccelerationDuration = 2.f;

	UPROPERTY(EditDefaultsOnly, Category = "Tank Movement")
	float AccelerationExponent = 2.f;

	UPROPERTY(EditDefaultsOnly, Category = "Tank Movement", meta = (ClampMin = "0.0"))
	float Speed = 5.f;

	UPROPERTY(Replicated)
	float AccelerationDurationElapsed = 0.f;

	UPROPERTY(Replicated)
	bool bIsMovingForward = true;

	UPROPERTY(Replicated)
	float LastDirectionChangedTime = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Tank Movement")
	float RotationRate = 2.f;

	UPROPERTY(EditDefaultsOnly, Category = "Tank|Camera")
	float MinPitch = 70.f;

	UPROPERTY(EditDefaultsOnly, Category = "Tank|Camera")
	float MaxPitch = 90.f;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	float SmokeSpeedModifier = 0.f;

	UPROPERTY(EditAnywhere, Category = "VFX")
	FCameraShakeData OnFireCameraShakeData;

	UPROPERTY(EditDefaultsOnly, Category = "Death")
	float SpectatorOffsetSpawnDistance = 100.f;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAmmoComponent> AmmoComponent;

	FTimerHandle ReduceSpeedTimerHandle;

public:
	float GetSpeed() const { return Speed; }
	float GetAccelerationDuration() const { return AccelerationDuration; }
	float GetElapsedTimeSinceLastDirectionChange() const { return AccelerationDurationElapsed - LastDirectionChangedTime; }

	bool IsMovingForward() const { return bIsMovingForward; }

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void ClientPlayVFXOnFire_Implementation() const override;

private:
	APlayerController* GetPlayerController() const;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	void SetupActions(UInputComponent* PlayerInputComponent);
	void SetupInputContext(const UInputMappingContext* InputMappingContext);
	void RemoveInputContext(const UInputMappingContext* InputMappingContext);

	void ShowCursor();
	void HideCursor();

	UFUNCTION(Server, Unreliable, WithValidation)
	void ServerMove(const float AxisValue, const float ElapsedTime);
	void Move(const FInputActionInstance& ActionData);
	float MoveReturningAccelerationValue_Internal(const float AxisValue, const float ElapsedTime);
	
	void OnMoveStopped();

	UFUNCTION(Server, Reliable)
	void ServerOnMoveStopped();

	UFUNCTION(Server, Unreliable, WithValidation)
	void ServerTurn(float AxisValue);
	void Turn_Internal(float AxisValue);

	bool IsValueWithinAxisValuesRange(const float AxisValue) const;

	void Turn(const FInputActionInstance& ActionData);
	void RotateCamera(const FInputActionInstance& ActionData);
	void ToggleAutoTarget();
	void FindAndLockTarget();
	void FindTarget(const APlayerController* PlayerController, FHitResult& HitResultOut) const;
	void RotateTurretMeshByCursor(const float DeltaSeconds);
	void RefreshCooldownWidget();
	void ResetAccelerationDurationElapsed();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastUpdateSmokeEffectSpeed(float SmokeSpeed);
	void ResetCooldownWidget() const;

	void ActivateMovementSound();

	void AdjustMovementComponentVolumeToSpeed(const float NewSpeed);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastSetMovementSoundVolume(const float Volume);

	void ResetMomentSoundVolume();
	void SetupReduceMovementVolumeTimer();
	void ScheduleCooldownResetOnNextTick();
	void ClearReduceSpeedTimer();
	void ReduceVolumeOverTime();

	void PlayOnFireCameraShake() const;

	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	virtual void SetActorTickEnabled(bool bEnabled) override;
	virtual void Destroyed() override;
	virtual void OnSuccessfulFire() const override;
	virtual bool CanFire() const override;

public:
	ATankPawn();
};
