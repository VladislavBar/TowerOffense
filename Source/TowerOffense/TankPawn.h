#pragma once

#include "Camera/CameraComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "TurretPawn.h"

#include "TankPawn.generated.h"

UCLASS()
class TOWEROFFENSE_API ATankPawn : public ATurretPawn
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(EditAnywhere, Category = "Tank|Camera")
	TObjectPtr<UCameraComponent> Camera;

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
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Tank Movement", meta = (ClampMin = "0.0"))
	float AccelerationDuration = 2.f;

	UPROPERTY(EditDefaultsOnly, Category = "Tank Movement")
	float AccelerationExponent = 2.f;

	UPROPERTY(EditDefaultsOnly, Category = "Tank Movement", meta = (ClampMin = "0.0"))
	float Speed = 5.f;
	float AccelerationDurationElapsed = 0.f;
	bool bIsMovingForward = false;

	UPROPERTY(EditDefaultsOnly, Category = "Tank Movement")
	float RotationRate = 2.f;

	UPROPERTY(EditDefaultsOnly, Category = "Tank|Camera")
	float MinPitch = 70.f;

	UPROPERTY(EditDefaultsOnly, Category = "Tank|Camera")
	float MaxPitch = 90.f;

public:
	float GetSpeed() const { return Speed; }
	float GetAccelerationDuration() const { return AccelerationDuration; }
	float GetAccelerationDurationElapsed() const { return AccelerationDurationElapsed; }

	bool IsMovingForward() const { return bIsMovingForward; }

private:
	APlayerController* GetPlayerController() const;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	void SetupActions(UInputComponent* PlayerInputComponent);
	void SetupInputContext();
	void ShowCursor();
	void HideCursor();

	void Move(const FInputActionInstance& ActionData);
	void Turn(const FInputActionInstance& ActionData);
	void RotateCamera(const FInputActionInstance& ActionData);
	void ToggleAutoTarget();
	void FindAndLockTarget();
	void FindTarget(const APlayerController* PlayerController, FHitResult& HitResultOut) const;
	void RotateTurretMeshByCursor(const float DeltaSeconds);
	void RefreshCooldownWidget();
	void ResetAccelerationDurationElapsed();

	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;

public:
	ATankPawn();
};
