#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "TurretPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
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

private:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	void Move(const FInputActionInstance& ActionData);
	void Turn(const FInputActionInstance& ActionData);
	
	UFUNCTION(BlueprintCallable)
	void Fire();

public:
	ATankPawn();
};
