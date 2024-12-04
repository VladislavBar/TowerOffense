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
DECLARE_LOG_CATEGORY_EXTERN(LogTankPawn, Log, All);

UENUM(BlueprintType)
enum class EActionType : uint8
{
	None,
	MoveForward,
	TurnRight,
	OnMoveStopped,
	RotateTurretMeshByCursor
};

USTRUCT()
struct FClientAction
{
	GENERATED_BODY()

	UPROPERTY()
	double Timestamp;

	virtual EActionType GetActionType()
	{
		unimplemented();
		return EActionType::None;
	}
};

USTRUCT()
struct FMoveForwardActionData : public FClientAction
{
	GENERATED_BODY()

	UPROPERTY()
	float DeltaTime;

	UPROPERTY()
	float AxisValue;

	UPROPERTY()
	float ElapsedTime;

	virtual EActionType GetActionType() override { return EActionType::MoveForward; }
};

USTRUCT()
struct FTurnRightActionData : public FClientAction
{
	GENERATED_BODY()

	UPROPERTY()
	float DeltaTime;

	UPROPERTY()
	float AxisValue;

	virtual EActionType GetActionType() override { return EActionType::TurnRight; }
};

USTRUCT()
struct FOnMoveStoppedActionData : public FClientAction
{
	GENERATED_BODY()

	virtual EActionType GetActionType() override { return EActionType::OnMoveStopped; }
};

USTRUCT()
struct FRotateTurretMeshByCursorActionData : public FClientAction
{
	GENERATED_BODY()

	UPROPERTY()
	float DeltaTime;

	UPROPERTY()
	FVector TargetPosition;

	virtual EActionType GetActionType() override { return EActionType::RotateTurretMeshByCursor; }
};

USTRUCT()
struct FPlayerStateData
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	FRotator Rotation;

	UPROPERTY()
	float AccelerationDurationElapsed;

	UPROPERTY()
	float LastDirectionChangedTime;

	UPROPERTY()
	bool bIsMovingForward;

	UPROPERTY()
	FRotator RelativeTurretRotation;
};

struct FPlayerActionAndStateData
{
	TSharedPtr<FClientAction> ActionData;
	FPlayerStateData PlayerStateData;
};

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
	TQueue<FPlayerActionAndStateData> PlayerActions;

	UPROPERTY(EditDefaultsOnly)
	float MaxDeltaTime = 0.05f;

	UPROPERTY(EditDefaultsOnly)
	float MaxReconcileError = 0.1f;

	UPROPERTY(ReplicatedUsing = OnRep_SmokeSpeed)
	float SmokeSpeed = 0.f;

	UPROPERTY(ReplicatedUsing = OnRep_MovementSoundVolume)
	float MovementSoundVolume = 0.f;

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

	void SetSmokeSpeed(const float NewSpeed);
	void SetMovementSound(const float NewSoundVolume);

	UFUNCTION()
	void OnRep_SmokeSpeed();

	UFUNCTION()
	void OnRep_MovementSoundVolume();

	UFUNCTION(Client, Reliable)
	void UpdateClientState(FPlayerStateData ServerPlayerStateData, const double Timestamp);

	bool ShouldReconcile(const FPlayerStateData& ServerPlayerStateData, const FPlayerActionAndStateData& ClientPlayerStateData) const;
	void SetActorState(const FPlayerStateData& PlayerStateData);

	bool DequeueUntilStateAndAction(FPlayerActionAndStateData& PlayerActionAndStateData, const double Timestamp);

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	void SetupActions(UInputComponent* PlayerInputComponent);
	void SetupInputContext(const UInputMappingContext* InputMappingContext);
	void RemoveInputContext(const UInputMappingContext* InputMappingContext);

	void ShowCursor();
	void HideCursor();

	void Move(const FInputActionInstance& ActionData);
	float ClientMoveReturningAccelerationValue(const float AxisValue, const float ElapsedTime);

	UFUNCTION(Server, Unreliable, WithValidation)
	void ServerVerifyMove(const FMoveForwardActionData ActionData);
	float PerformMoveReturningAccelerationValue(const float AxisValue, const float ElapsedTime, const float DeltaSeconds);

	void OnMoveStopped();
	void PerformOnMoveStopped();

	UFUNCTION(Server, Unreliable)
	void ServerVerifyOnMoveStopped(FOnMoveStoppedActionData ActionData);

	UFUNCTION(Server, Reliable)
	void ServerOnMoveStopped();
	float ClampDeltaTime(const float DeltaTime) const;

	void Turn_Internal(float AxisValue, float DeltaSeconds);
	void PerformTurn(float AxisValue, float DeltaSeconds);
	FRotator CalculateRotation(float AxisValue, float DeltaSeconds);

	UFUNCTION(Server, Unreliable, WithValidation)
	void ServerVerifyRotation(const FTurnRightActionData ActionData);

	bool IsValueWithinAxisValuesRange(const float AxisValue) const;
	UFUNCTION(Client, Reliable)
	void ClientReconcileState(const FPlayerStateData& ServerPlayerStateData, const double ServerTimestamp);
	void PerformReconcileAction(const FPlayerStateData& ServerPlayerStateData, const FPlayerActionAndStateData& ClientPlayerStateData);

	FPlayerStateData GetPlayerStateData() const;
	TSharedPtr<FTurnRightActionData> GetTurnRightActionData(const float AxisValue, const float DeltaSeconds, const double Timestamp) const;
	TSharedPtr<FMoveForwardActionData> GetMoveForwardActionData(
		const float AxisValue, const float AccelerationValue, const double Timestamp, const float DeltaTime) const;
	TSharedPtr<FOnMoveStoppedActionData> GetOnMoveStoppedActionData(const double Timestamp) const;
	TSharedPtr<FRotateTurretMeshByCursorActionData> GetRotateTurretMeshByCursorActionData(
		const float DeltaSeconds, const FVector& TargetPosition, const double Timestamp) const;

	void Turn(const FInputActionInstance& ActionData);
	void RotateCamera(const FInputActionInstance& ActionData);
	void ToggleAutoTarget();
	void FindAndLockTarget();
	void FindTarget(const APlayerController* PlayerController, FHitResult& HitResultOut) const;
	void RotateTurretMeshByCursor(const float DeltaSeconds);

	UFUNCTION(Server, Unreliable)
	void ServerVerifyTurretRotation(const FRotateTurretMeshByCursorActionData ActionData);
	virtual void ClientRotateTurretMeshToLocation(const float DeltaSeconds, const FVector& Location, bool bInstantRotation) override;

	void RefreshCooldownWidget();
	void ResetAccelerationDurationElapsed();

	void UpdateSmokeEffectSpeed();
	void ResetCooldownWidget() const;

	void OnSetActorTickEnabled(const bool bEnabled);

	void ActivateMovementSound();

	void AdjustMovementComponentVolumeToSpeed(const float NewSpeed);

	void SetMovementSoundVolumeInComponent(const float Volume);

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
