#include "TankPawn.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "TankPlayerController.h"

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
		EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Completed, this, &ATankPawn::ResetAccelerationDurationElapsed);
	}

	if (IsValid(TurnRightAction))
	{
		EnhancedInputComponent->BindAction(TurnRightAction, ETriggerEvent::Triggered, this, &ATankPawn::Turn);
	}

	if (IsValid(FireAction))
	{
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ATankPawn::Fire);
	}

	if (IsValid(RotateCameraAction))
	{
		EnhancedInputComponent->BindAction(RotateCameraAction, ETriggerEvent::Triggered, this,
			&ATankPawn::RotateCamera);
	}

	if (IsValid(SetTargetAction))
	{
		EnhancedInputComponent->BindAction(SetTargetAction, ETriggerEvent::Started, this,
			&ATankPawn::ToggleAutoTarget);
	}
}

void ATankPawn::SetupInputContext()
{
	if (!IsValid(InputMappingContext)) return;

	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	const ULocalPlayer* LocalPlayer = World->GetFirstLocalPlayerFromController();
	if (!IsValid(LocalPlayer)) return;

	UEnhancedInputLocalPlayerSubsystem* EnhancedInputLocalPlayerSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!IsValid(EnhancedInputLocalPlayerSubsystem)) return;

	EnhancedInputLocalPlayerSubsystem->AddMappingContext(InputMappingContext, 0);
}

void ATankPawn::ShowCursor()
{
	APlayerController* PlayerController = GetPlayerController();
	if (!IsValid(PlayerController)) return;

	PlayerController->bShowMouseCursor = true;
}

void ATankPawn::HideCursor()
{
	APlayerController* PlayerController = GetPlayerController();
	if (!IsValid(PlayerController)) return;

	PlayerController->bShowMouseCursor = false;
}

void ATankPawn::Move(const FInputActionInstance& ActionData)
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	const float AxisValue = ActionData.GetValue().Get<float>();

	AccelerationDurationElapsed = ActionData.GetElapsedTime();

	if(bIsMovingForward && AxisValue < 0.f || !bIsMovingForward && AxisValue > 0.f)
	{
		LastDirectionChangedTime = AccelerationDurationElapsed;
	}

	bIsMovingForward = AxisValue > 0.f;
	if (AccelerationDuration <= KINDA_SMALL_NUMBER)
	{
		AddActorLocalOffset(FVector(Speed * AxisValue, 0.f, 0.f));
		return;
	}

	const float AccelerationProgress = FMath::Clamp((ActionData.GetElapsedTime() - LastDirectionChangedTime) / AccelerationDuration, 0.f, 1.f);
	const float AccelerationValue = FMath::InterpEaseIn(0.f, 1.f, AccelerationProgress,
		AccelerationExponent);
	AddActorLocalOffset(FVector(Speed * AccelerationValue * AxisValue * World->GetDeltaSeconds(), 0.f, 0.f), true);
}

void ATankPawn::Turn(const FInputActionInstance& ActionData)
{
	const float AxisValue = ActionData.GetValue().Get<float>();
	AddActorLocalRotation(FRotator(0.f, AxisValue * RotationRate, 0.f));
}

void ATankPawn::RotateCamera(const FInputActionInstance& ActionData)
{
	if (!IsValid(SpringArm)) return;

	const APlayerController* PlayerController = GetPlayerController();
	if (!IsValid(PlayerController) || PlayerController->bShowMouseCursor) return;

	const FVector2D RotationVector2D = ActionData.GetValue().Get<FVector2D>();
	const FVector RotationVector = FVector(0.f, RotationVector2D.Y, RotationVector2D.X);
	FRotator NewRotation = SpringArm->GetRelativeRotation() + FRotator::MakeFromEuler(RotationVector);
	NewRotation.Pitch = FMath::ClampAngle(NewRotation.Pitch, MinPitch, MaxPitch);
	SpringArm->SetRelativeRotation(NewRotation);
}

void ATankPawn::ToggleAutoTarget()
{
	if (bLockTarget)
	{
		bLockTarget = false;
		if (IsValid(GEngine))
		{
			GEngine->AddOnScreenDebugMessage(1, 99999.f, FColor::Red, FString::Printf(TEXT("Target unlocked! Following the mouse cursor...")));
		}

		return;
	}

	FindAndLockTarget();
	bLockTarget = true;
}

void ATankPawn::FindAndLockTarget()
{
	const APlayerController* PlayerController = GetPlayerController();
	if (!IsValid(PlayerController)) return;

	FHitResult HitResult;
	FindTarget(PlayerController, HitResult);
	DrawDebugSphere(GetWorld(), HitResult.Location, 50.f, 12, FColor::Green, false, 5.f);
	SetTargetLocation(HitResult.Location);
	bLockTarget = true;

	if (IsValid(GEngine))
	{
		GEngine->AddOnScreenDebugMessage(1, 99999.f, FColor::Red, FString::Printf(TEXT("Target locked!")));
	}
}

void ATankPawn::FindTarget(const APlayerController* PlayerController, FHitResult& HitResultOut) const
{
	if (!IsValid(PlayerController)) return;

	FVector2D MousePosition = FVector2D::ZeroVector;
	PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y);

	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);

	PlayerController->GetHitResultAtScreenPosition(MousePosition, ECC_Visibility, CollisionQueryParams, HitResultOut);
}

void ATankPawn::RotateTurretMeshByCursor(const float DeltaSeconds)
{
	if (bLockTarget || !IsValid(TurretMesh)) return;

	const APlayerController* PlayerController = GetPlayerController();
	if (!IsValid(PlayerController)) return;

	FHitResult HitResult;
	FindTarget(PlayerController, HitResult);
	RotateTurretMeshToLocation(DeltaSeconds, HitResult.Location);

	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	DrawDebugSphere(GetWorld(), HitResult.Location, 50.f, 12, FColor::Red, false, 0.f);
}

void ATankPawn::RefreshCooldownWidget()
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	ATankPlayerController* PlayerController = Cast<ATankPlayerController>(World->GetFirstPlayerController());
	if (!IsValid(PlayerController)) return;

	const float RemainingCooldownTime = World->GetTimerManager().GetTimerRemaining(FireCooldownTimerHandle);
	PlayerController->RefreshCooldownWidget(RemainingCooldownTime);
}

void ATankPawn::ResetAccelerationDurationElapsed()
{
	AccelerationDurationElapsed = 0.f;
	LastDirectionChangedTime = 0.f;
}

void ATankPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	RotateTurretMeshByCursor(DeltaSeconds);
	RefreshCooldownWidget();
}

void ATankPawn::BeginPlay()
{
	Super::BeginPlay();

	HideCursor();
}

APlayerController* ATankPawn::GetPlayerController() const
{
	UWorld* World = GetWorld();
	if (!IsValid(World)) return nullptr;

	return World->GetFirstPlayerController();
}
