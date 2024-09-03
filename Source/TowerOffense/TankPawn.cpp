#include "TankPawn.h"
#include "EnhancedInputComponent.h"

ATankPawn::ATankPawn()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

void ATankPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent) return;

	if (IsValid(MoveForwardAction))
	{
		EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Started, this, &ATankPawn::Move);
	}

	if (IsValid(TurnRightAction))
	{
		EnhancedInputComponent->BindAction(TurnRightAction, ETriggerEvent::Started, this, &ATankPawn::Turn);
	}

	if (IsValid(FireAction))
	{
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &ATankPawn::Fire);
	}
}

void ATankPawn::Move(const FInputActionInstance& ActionData)
{
}

void ATankPawn::Turn(const FInputActionInstance& ActionData)
{
}

void ATankPawn::Fire()
{
}
