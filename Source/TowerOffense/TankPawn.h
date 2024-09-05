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

	UPROPERTY(EditDefaultsOnly)
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

	UPROPERTY(EditDefaultsOnly, Category = "Tank Movement")
	float RotationRate = 2.f;

private:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	void SetupActions(UInputComponent* PlayerInputComponent);
	void SetupInputContext();
	void ToggleCursor();

	void Move(const FInputActionInstance& ActionData);
	void Turn(const FInputActionInstance& ActionData);
	void RotateCamera(const FInputActionInstance& ActionData);
	void SetTarget();

	UFUNCTION(BlueprintCallable)
	void Fire();

public:
	ATankPawn();
};
