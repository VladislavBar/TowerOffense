#include "TankPawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

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

	SetupActions(PlayerInputComponent);
	SetupInputContext();
}

void ATankPawn::SetupActions(UInputComponent* PlayerInputComponent)
{
	if (!IsValid(PlayerInputComponent)) return;

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!IsValid(EnhancedInputComponent)) return;

	if (IsValid(MoveForwardAction))
	{
		EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &ATankPawn::Move);
	}

	if (IsValid(TurnRightAction))
	{
		EnhancedInputComponent->BindAction(TurnRightAction, ETriggerEvent::Triggered, this, &ATankPawn::Turn);
	}

	if (IsValid(FireAction))
	{
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &ATankPawn::Fire);
	}

	if (IsValid(RotateCameraAction))
	{
		EnhancedInputComponent->BindAction(RotateCameraAction, ETriggerEvent::Triggered, this,
		                                   &ATankPawn::RotateCamera);
	}

	if (IsValid(SetTargetAction))
	{
		EnhancedInputComponent->BindAction(SetTargetAction, ETriggerEvent::Started, this,
		                                   &ATankPawn::SetTarget);
	}

	if (IsValid(ToggleCursorAction))
	{
		EnhancedInputComponent->BindAction(ToggleCursorAction, ETriggerEvent::Started, this,
		                                   &ATankPawn::ToggleCursor);
	}
}

void ATankPawn::SetupInputContext()
{
	if (!IsValid(InputMappingContext)) return;

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!IsValid(LocalPlayer)) return;

	UEnhancedInputLocalPlayerSubsystem* EnhancedInputLocalPlayerSubsystem = LocalPlayer->GetSubsystem<
		UEnhancedInputLocalPlayerSubsystem>();
	if (!IsValid(EnhancedInputLocalPlayerSubsystem)) return;

	EnhancedInputLocalPlayerSubsystem->AddMappingContext(InputMappingContext, 0);
}

void ATankPawn::ToggleCursor()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!IsValid(PlayerController)) return;

	PlayerController->bShowMouseCursor = !PlayerController->bShowMouseCursor;
}

void ATankPawn::Move(const FInputActionInstance& ActionData)
{
	const float AxisValue = ActionData.GetValue().Get<float>();

	if (AccelerationDuration <= KINDA_SMALL_NUMBER)
	{
		AddActorLocalOffset(FVector(Speed * AxisValue, 0.f, 0.f));
		return;
	}

	const float AccelerationProgress = FMath::Clamp(ActionData.GetElapsedTime() / AccelerationDuration, 0.f, 1.f);
	const float AccelerationValue = FMath::InterpEaseIn(0.f, AccelerationDuration, AccelerationProgress,
	                                                    AccelerationExponent);
	AddActorLocalOffset(FVector(0.f, Speed * AccelerationValue * AxisValue, 0.f), true);
}

void ATankPawn::Turn(const FInputActionInstance& ActionData)
{
	const float AxisValue = ActionData.GetValue().Get<float>();
	AddActorLocalRotation(FRotator(0.f, AxisValue * RotationRate, 0.f));
}

void ATankPawn::RotateCamera(const FInputActionInstance& ActionData)
{
	const FVector2D RotationVector2D = ActionData.GetValue().Get<FVector2D>();
	const FVector RotationVector = FVector(0.f, 0.f, RotationVector2D.X);
	SpringArm->AddWorldRotation(FRotator::MakeFromEuler(RotationVector));
}

void ATankPawn::SetTarget()
{
	const APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!IsValid(PlayerController)) return;

	FHitResult HitResult;
	PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
	DrawDebugSphere(GetWorld(), HitResult.Location, 50.f, 12, FColor::Red, false, 5.f);
	SetTargetLocation(HitResult.Location);
}

void ATankPawn::Fire()
{
}
