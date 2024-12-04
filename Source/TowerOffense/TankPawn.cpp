#include "TankPawn.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "NiagaraComponent.h"
#include "TankPlayerController.h"
#include "Components/AudioComponent.h"
#include "GameFramework/SpectatorPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogTankPawn);

ATankPawn::ATankPawn()
{
	bReplicates = true;
	SetReplicateMovement(true);
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArm);

	VehicleSmokeEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("VehicleSmokeEffect"));
	VehicleSmokeEffect->SetupAttachment(RootComponent);

	MovementSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("MovementSoundComponent"));
	MovementSoundComponent->SetupAttachment(RootComponent);
	MovementSoundComponent->bAutoActivate = false;

	AmmoComponent = CreateDefaultSubobject<UAmmoComponent>(TEXT("AmmoComponent"));
	AmmoComponent->SetIsReplicated(true);

	RootComponent->SetIsReplicated(true);
}

void ATankPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	SetupActions(PlayerInputComponent);
	SetupInputContext(StartDelayMappingContext);
}

void ATankPawn::SetupActions(UInputComponent* PlayerInputComponent)
{
	if (!IsValid(PlayerInputComponent)) return;

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!IsValid(EnhancedInputComponent)) return;

	if (IsValid(MoveForwardAction))
	{
		EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &ATankPawn::Move);
		EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Completed, this, &ATankPawn::OnMoveStopped);
	}

	if (IsValid(TurnRightAction))
	{
		EnhancedInputComponent->BindAction(TurnRightAction, ETriggerEvent::Triggered, this, &ATankPawn::Turn);
	}

	if (IsValid(FireAction))
	{
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ATankPawn::ClientFire);
	}

	if (IsValid(RotateCameraAction))
	{
		EnhancedInputComponent->BindAction(RotateCameraAction, ETriggerEvent::Triggered, this, &ATankPawn::RotateCamera);
	}

	if (IsValid(SetTargetAction))
	{
		EnhancedInputComponent->BindAction(SetTargetAction, ETriggerEvent::Started, this, &ATankPawn::ToggleAutoTarget);
	}
}

void ATankPawn::SetupInputContext(const UInputMappingContext* InputMappingContext)
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

void ATankPawn::RemoveInputContext(const UInputMappingContext* InputMappingContext)
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	const ULocalPlayer* LocalPlayer = World->GetFirstLocalPlayerFromController();
	if (!IsValid(LocalPlayer)) return;

	UEnhancedInputLocalPlayerSubsystem* EnhancedInputLocalPlayerSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!IsValid(EnhancedInputLocalPlayerSubsystem)) return;

	EnhancedInputLocalPlayerSubsystem->RemoveMappingContext(InputMappingContext);
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

void ATankPawn::OnRep_SmokeSpeed()
{
	UpdateSmokeEffectSpeed();
}

void ATankPawn::OnRep_MovementSoundVolume()
{
	AdjustMovementComponentVolumeToSpeed(MovementSoundVolume);
}

void ATankPawn::Move(const FInputActionInstance& ActionData)
{
	if (!IsLocallyControlled()) return;

	const float AxisValue = ActionData.GetValue().Get<float>();
	const float ElapsedTime = ActionData.GetElapsedTime();

	ClientMoveReturningAccelerationValue(AxisValue, ElapsedTime);
}

float ATankPawn::ClientMoveReturningAccelerationValue(const float AxisValue, const float ElapsedTime)
{
	const UWorld* World = GetWorld();
	if (!World) return 0.f;

	const float DeltaSeconds = World->GetDeltaSeconds();
	const float Acceleration = PerformMoveReturningAccelerationValue(AxisValue, ElapsedTime, DeltaSeconds);

	if (!HasAuthority())
	{
		const TSharedPtr<FMoveForwardActionData> ActionData = GetMoveForwardActionData(AxisValue, ElapsedTime, World->GetTimeSeconds(), DeltaSeconds);
		FPlayerActionAndStateData PlayerActionAndStateData;
		PlayerActionAndStateData.PlayerStateData = GetPlayerStateData();
		PlayerActionAndStateData.ActionData = ActionData;

		PlayerActions.Enqueue(PlayerActionAndStateData);
		ServerVerifyMove(*ActionData);
	}

	return Acceleration;
}

bool ATankPawn::ServerVerifyMove_Validate(const FMoveForwardActionData ActionData)
{
	return IsValueWithinAxisValuesRange(ActionData.AxisValue) && ActionData.ElapsedTime >= 0.f;
}

void ATankPawn::ServerVerifyMove_Implementation(const FMoveForwardActionData ActionData)
{
	const float DeltaTime = ClampDeltaTime(ActionData.DeltaTime);
	PerformMoveReturningAccelerationValue(ActionData.AxisValue, ActionData.ElapsedTime, DeltaTime);
	const FPlayerStateData PlayerStateData = GetPlayerStateData();
	UpdateClientState(PlayerStateData, ActionData.Timestamp);
}

float ATankPawn::PerformMoveReturningAccelerationValue(const float AxisValue, const float ElapsedTime, const float DeltaSeconds)
{
	AccelerationDurationElapsed = ElapsedTime;
	if (bIsMovingForward && AxisValue < 0.f || !bIsMovingForward && AxisValue >= 0.f)
	{
		LastDirectionChangedTime = AccelerationDurationElapsed;
	}

	bIsMovingForward = AxisValue >= 0.f;
	if (AccelerationDuration <= KINDA_SMALL_NUMBER)
	{
		AddActorLocalOffset(FVector(Speed * AxisValue, 0.f, 0.f), true);
		return 1.f;
	}

	const float AccelerationProgress = FMath::Clamp((ElapsedTime - LastDirectionChangedTime) / AccelerationDuration, 0.f, 1.f);
	const float AccelerationValue = FMath::InterpEaseIn(0.f, 1.f, AccelerationProgress, AccelerationExponent);

	AddActorLocalOffset(FVector(Speed * AccelerationValue * AxisValue * DeltaSeconds, 0.f, 0.f), true);

	SetSmokeSpeed(Speed * AccelerationValue);
	SetMovementSound(AccelerationValue);

	return AccelerationValue;
}

void ATankPawn::OnMoveStopped()
{
	if (!IsLocallyControlled()) return;

	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	PerformOnMoveStopped();
	if (!HasAuthority())
	{
		const TSharedPtr<FOnMoveStoppedActionData> ActionData = GetOnMoveStoppedActionData(World->GetTimeSeconds());

		FPlayerActionAndStateData PlayerActionAndStateData;
		PlayerActionAndStateData.PlayerStateData = GetPlayerStateData();
		PlayerActions.Enqueue(PlayerActionAndStateData);

		ServerVerifyOnMoveStopped(*ActionData);
	}
}

void ATankPawn::PerformOnMoveStopped()
{
	SetSmokeSpeed(0.f);
	ResetAccelerationDurationElapsed();
	SetupReduceMovementVolumeTimer();
	bIsMovingForward = true;
}

void ATankPawn::ServerVerifyOnMoveStopped_Implementation(FOnMoveStoppedActionData ActionData)
{
	PerformOnMoveStopped();
	const FPlayerStateData PlayerStateData = GetPlayerStateData();
	UpdateClientState(PlayerStateData, ActionData.Timestamp);
}

void ATankPawn::ServerOnMoveStopped_Implementation()
{
	ResetAccelerationDurationElapsed();
	SetSmokeSpeed(0.f);
	SetupReduceMovementVolumeTimer();
	bIsMovingForward = true;
}

void ATankPawn::PerformReconcileAction(const FPlayerStateData& ServerPlayerStateData, const FPlayerActionAndStateData& ClientPlayerStateData)
{
	if (!ClientPlayerStateData.ActionData.IsValid()) return;

	switch (ClientPlayerStateData.ActionData->GetActionType())
	{
		case EActionType::MoveForward:
		{
			const TSharedPtr<FMoveForwardActionData> MoveForwardActionData = StaticCastSharedPtr<FMoveForwardActionData>(ClientPlayerStateData.ActionData);
			if (!MoveForwardActionData.IsValid()) return;

			PerformMoveReturningAccelerationValue(MoveForwardActionData->AxisValue, MoveForwardActionData->ElapsedTime, MoveForwardActionData->DeltaTime);
			return;
		}

		case EActionType::TurnRight:
		{
			const TSharedPtr<FTurnRightActionData> TurnRightActionData = StaticCastSharedPtr<FTurnRightActionData>(ClientPlayerStateData.ActionData);
			if (!TurnRightActionData.IsValid()) return;

			return PerformTurn(TurnRightActionData->AxisValue, TurnRightActionData->DeltaTime);
		}
		case EActionType::OnMoveStopped:
		{
			return PerformOnMoveStopped();
		}
		case EActionType::RotateTurretMeshByCursor:
		{
			const TSharedPtr<FRotateTurretMeshByCursorActionData> RotateTurretMeshByCursorActionData =
				StaticCastSharedPtr<FRotateTurretMeshByCursorActionData>(ClientPlayerStateData.ActionData);
			if (!RotateTurretMeshByCursorActionData.IsValid()) return;

			return RotateTurretMeshToLocation_Internal(
				RotateTurretMeshByCursorActionData->DeltaTime, RotateTurretMeshByCursorActionData->TargetPosition, false);
		}

		default:
		{
			UE_LOG(LogTankPawn, Error, TEXT("Unknown action type! %d"), static_cast<int32>(ClientPlayerStateData.ActionData->GetActionType()));
		}
	}
}

FPlayerStateData ATankPawn::GetPlayerStateData() const
{
	FPlayerStateData PlayerStateData;

	PlayerStateData.Location = GetActorLocation();
	PlayerStateData.Rotation = GetActorRotation().Clamp();
	PlayerStateData.AccelerationDurationElapsed = AccelerationDurationElapsed;
	PlayerStateData.LastDirectionChangedTime = LastDirectionChangedTime;
	PlayerStateData.bIsMovingForward = bIsMovingForward;
	PlayerStateData.RelativeTurretRotation = TurretMesh->GetRelativeRotation();

	return PlayerStateData;
}

TSharedPtr<FTurnRightActionData> ATankPawn::GetTurnRightActionData(const float AxisValue, const float DeltaSeconds, const double Timestamp) const
{
	TSharedPtr<FTurnRightActionData> ActionData = MakeShared<FTurnRightActionData>();

	ActionData->AxisValue = AxisValue;
	ActionData->DeltaTime = DeltaSeconds;
	ActionData->Timestamp = Timestamp;

	return ActionData;
}

TSharedPtr<FMoveForwardActionData> ATankPawn::GetMoveForwardActionData(
	const float AxisValue, const float ElapsedTime, const double Timestamp, const float DeltaTime) const
{
	TSharedPtr<FMoveForwardActionData> ActionData = MakeShared<FMoveForwardActionData>();

	ActionData->DeltaTime = DeltaTime;
	ActionData->AxisValue = AxisValue;
	ActionData->ElapsedTime = ElapsedTime;
	ActionData->Timestamp = Timestamp;

	return ActionData;
}
TSharedPtr<FOnMoveStoppedActionData> ATankPawn::GetOnMoveStoppedActionData(const double Timestamp) const
{
	TSharedPtr<FOnMoveStoppedActionData> ActionData = MakeShared<FOnMoveStoppedActionData>();
	ActionData->Timestamp = Timestamp;
	return ActionData;
}

TSharedPtr<FRotateTurretMeshByCursorActionData> ATankPawn::GetRotateTurretMeshByCursorActionData(
	const float DeltaSeconds, const FVector& TargetPosition, const double Timestamp) const
{
	TSharedPtr<FRotateTurretMeshByCursorActionData> ActionData = MakeShared<FRotateTurretMeshByCursorActionData>();

	ActionData->DeltaTime = DeltaSeconds;
	ActionData->TargetPosition = TargetPosition;
	ActionData->Timestamp = Timestamp;

	return ActionData;
}

void ATankPawn::Turn(const FInputActionInstance& ActionData)
{
	const float AxisValue = ActionData.GetValue().Get<float>();
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	if (IsLocallyControlled())
	{
		Turn_Internal(AxisValue, World->GetDeltaSeconds());
	}
}

float ATankPawn::ClampDeltaTime(const float DeltaTime) const
{
	return FMath::Clamp(DeltaTime, 0.f, MaxDeltaTime);
}

void ATankPawn::Turn_Internal(float AxisValue, float DeltaSeconds)
{
	PerformTurn(AxisValue, DeltaSeconds);

	const UWorld* World = GetWorld();
	if (IsLocallyControlled() && IsValid(World) && !HasAuthority())
	{
		const FPlayerStateData PlayerStateData = GetPlayerStateData();
		TSharedPtr<FTurnRightActionData> ActionData = GetTurnRightActionData(AxisValue, DeltaSeconds, World->GetTimeSeconds());

		FPlayerActionAndStateData PlayerActionAndStateData;
		PlayerActionAndStateData.ActionData = ActionData;
		PlayerActionAndStateData.PlayerStateData = PlayerStateData;

		PlayerActions.Enqueue(PlayerActionAndStateData);
		ServerVerifyRotation(*ActionData);
	}
}

void ATankPawn::PerformTurn(float AxisValue, float DeltaSeconds)
{
	if (!bIsMovingForward) AxisValue *= -1.f;

	const FRotator NewRotation = CalculateRotation(AxisValue, DeltaSeconds);
	AddActorLocalRotation(NewRotation);
}

FRotator ATankPawn::CalculateRotation(float AxisValue, float DeltaSeconds)
{
	return FRotator(0.f, AxisValue * RotationRate * DeltaSeconds, 0.f);
}

bool ATankPawn::IsValueWithinAxisValuesRange(const float AxisValue) const
{
	// Is in range [-1, 1]
	return FMath::Abs(AxisValue) >= KINDA_SMALL_NUMBER && FMath::Abs(AxisValue) - 1.f <= KINDA_SMALL_NUMBER;
}

void ATankPawn::UpdateClientState_Implementation(FPlayerStateData ServerPlayerStateData, const double Timestamp)
{
	FPlayerActionAndStateData* StateAndAction = PlayerActions.Peek();
	if (StateAndAction == nullptr)
	{
		SetActorState(ServerPlayerStateData);
		return;
	}

	FPlayerActionAndStateData StateAndActionData;
	const bool HasFoundAction = DequeueUntilStateAndAction(StateAndActionData, Timestamp);
	if (!HasFoundAction || ShouldReconcile(ServerPlayerStateData, StateAndActionData))
	{
		return ClientReconcileState(ServerPlayerStateData, Timestamp);
	}
}

bool ATankPawn::ShouldReconcile(const FPlayerStateData& ServerPlayerStateData, const FPlayerActionAndStateData& ClientStateAndAction) const
{
	const TSharedPtr<FClientAction> ActionData = ClientStateAndAction.ActionData;
	const FPlayerStateData& ClientPlayerStateData = ClientStateAndAction.PlayerStateData;

	return !ActionData.IsValid() || !ServerPlayerStateData.Rotation.Equals(ClientPlayerStateData.Rotation, MaxReconcileError)
		|| !ServerPlayerStateData.Location.Equals(ClientPlayerStateData.Location, MaxReconcileError)
		|| ServerPlayerStateData.bIsMovingForward != ClientPlayerStateData.bIsMovingForward
		|| FMath::Abs(ServerPlayerStateData.LastDirectionChangedTime - ClientPlayerStateData.LastDirectionChangedTime) > UE_KINDA_SMALL_NUMBER
		|| FMath::Abs(ServerPlayerStateData.AccelerationDurationElapsed - ClientPlayerStateData.AccelerationDurationElapsed) > UE_KINDA_SMALL_NUMBER;
}

void ATankPawn::SetActorState(const FPlayerStateData& PlayerStateData)
{
	SetActorLocationAndRotation(PlayerStateData.Location, PlayerStateData.Rotation);
	AccelerationDurationElapsed = PlayerStateData.AccelerationDurationElapsed;
	LastDirectionChangedTime = PlayerStateData.LastDirectionChangedTime;
	bIsMovingForward = PlayerStateData.bIsMovingForward;
	TurretMesh->SetRelativeRotation(PlayerStateData.RelativeTurretRotation);
}

bool ATankPawn::DequeueUntilStateAndAction(FPlayerActionAndStateData& PlayerActionAndStateData, const double Timestamp)
{
	FPlayerActionAndStateData StateAndAction;
	while (PlayerActions.Dequeue(StateAndAction))
	{
		TSharedPtr<FClientAction> ActionData = StateAndAction.ActionData;
		if (!ActionData.IsValid()) continue;
		if (FMath::Abs(ActionData->Timestamp - Timestamp) <= UE_KINDA_SMALL_NUMBER)
		{
			PlayerActionAndStateData = StateAndAction;
			return true;
		};

		if (!ActionData.IsValid() || ActionData->Timestamp < Timestamp) continue;
		break;
	}

	return false;
}

void ATankPawn::ClientReconcileState_Implementation(const FPlayerStateData& ServerPlayerStateData, const double ServerTimestamp)
{
	TQueue<FPlayerActionAndStateData> TempQueue;

	SetActorState(ServerPlayerStateData);

	FPlayerActionAndStateData StateAndActionData;
	// Dequeue until we find the action with the same timestamp
	while (PlayerActions.Dequeue(StateAndActionData))
	{
		TSharedPtr<FClientAction> ActionData = StateAndActionData.ActionData;
		if (!ActionData.IsValid() || ActionData->Timestamp < ServerTimestamp) continue;

		PerformReconcileAction(ServerPlayerStateData, StateAndActionData);
		StateAndActionData.PlayerStateData = GetPlayerStateData();
		TempQueue.Enqueue(StateAndActionData);
	}

	while (TempQueue.Dequeue(StateAndActionData))
	{
		PlayerActions.Enqueue(StateAndActionData);
	}
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

	int32 ViewportWidth = 0;
	int32 ViewportHeight = 0;
	PlayerController->GetViewportSize(ViewportWidth, ViewportHeight);

	const FVector2D CenterOfTheScreen = FVector2D(ViewportWidth / 2, ViewportHeight / 2);
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);

	// TODO: replace with crosshair position instead of the center of the screen
	PlayerController->GetHitResultAtScreenPosition(CenterOfTheScreen, ECC_Visibility, CollisionQueryParams, HitResultOut);
}

void ATankPawn::RotateTurretMeshByCursor(const float DeltaSeconds)
{
	if (bLockTarget || !IsValid(TurretMesh)) return;

	const APlayerController* PlayerController = GetPlayerController();
	if (!IsValid(PlayerController)) return;

	FHitResult HitResult;
	FindTarget(PlayerController, HitResult);
	RotateTurretMeshToLocation(DeltaSeconds, HitResult.Location);
}

void ATankPawn::ServerVerifyTurretRotation_Implementation(const FRotateTurretMeshByCursorActionData ActionData)
{
	const float DeltaTime = ClampDeltaTime(ActionData.DeltaTime);
	RotateTurretMeshToLocation_Internal(DeltaTime, ActionData.TargetPosition, false);
	const FPlayerStateData PlayerStateData = GetPlayerStateData();
	UpdateClientState(PlayerStateData, ActionData.Timestamp);
}

void ATankPawn::ClientRotateTurretMeshToLocation(const float DeltaSeconds, const FVector& Location, bool bInstantRotation)
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	Super::ClientRotateTurretMeshToLocation(DeltaSeconds, Location, bInstantRotation);

	if (!HasAuthority())
	{
		TSharedPtr<FRotateTurretMeshByCursorActionData> ActionData = GetRotateTurretMeshByCursorActionData(DeltaSeconds, Location, World->GetTimeSeconds());

		FPlayerActionAndStateData PlayerActionAndStateData;
		PlayerActionAndStateData.PlayerStateData = GetPlayerStateData();
		PlayerActionAndStateData.ActionData = ActionData;

		PlayerActions.Enqueue(PlayerActionAndStateData);

		ServerVerifyTurretRotation(*ActionData);
	}
}

void ATankPawn::RefreshCooldownWidget()
{
	const ATankPlayerController* PlayerController = Cast<ATankPlayerController>(GetPlayerController());
	if (!IsValid(PlayerController)) return;

	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	const float RemainingCooldownTime = World->GetTimerManager().GetTimerRemaining(FireCooldownTimerHandle);
	OnCooldownTickDelegate.Broadcast(RemainingCooldownTime);
}

void ATankPawn::ResetAccelerationDurationElapsed()
{
	AccelerationDurationElapsed = 0.f;
	LastDirectionChangedTime = 0.f;
}

bool ATankPawn::ServerVerifyRotation_Validate(const FTurnRightActionData ActionData)
{
	return IsValueWithinAxisValuesRange(ActionData.AxisValue);
}

void ATankPawn::ServerVerifyRotation_Implementation(FTurnRightActionData ActionData)
{
	const float DeltaTime = ClampDeltaTime(ActionData.DeltaTime);
	PerformTurn(ActionData.AxisValue, DeltaTime);
	const FPlayerStateData PlayerStateData = GetPlayerStateData();
	UpdateClientState(PlayerStateData, ActionData.Timestamp);
}

void ATankPawn::UpdateSmokeEffectSpeed()
{
	if (!IsValid(VehicleSmokeEffect)) return;

	VehicleSmokeEffect->SetFloatParameter("Speed", SmokeSpeed * SmokeSpeedModifier);
}

void ATankPawn::ResetCooldownWidget() const
{
	OnCooldownTickDelegate.Broadcast(0.f);
}

void ATankPawn::OnSetActorTickEnabled(const bool bEnabled)
{
	if (bEnabled)
	{
		SetupInputContext(GameControlInputMappingContext);
		RemoveInputContext(StartDelayMappingContext);
	}
	else
	{
		SetupInputContext(StartDelayMappingContext);
		RemoveInputContext(GameControlInputMappingContext);
	}
}

void ATankPawn::ActivateMovementSound()
{
	if (!IsValid(MovementSoundComponent)) return;
	if (IsLocallyControlled())
	{
		MovementSoundComponent->AttenuationOverrides.bAttenuate = false;
	}

	MovementSoundComponent->Activate();
}

void ATankPawn::AdjustMovementComponentVolumeToSpeed(const float NewSpeed)
{
	SetMovementSoundVolumeInComponent(NewSpeed * MovementSoundVolumeMultiplier);
}

void ATankPawn::SetMovementSoundVolumeInComponent(const float Volume)
{
	if (!IsValid(MovementSoundComponent)) return;

	MovementSoundComponent->SetVolumeMultiplier(Volume);
}

void ATankPawn::ResetMomentSoundVolume()
{
	SetMovementSoundVolumeInComponent(DefaultMovementSoundVolume);
}

void ATankPawn::SetupReduceMovementVolumeTimer()
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	World->GetTimerManager().SetTimer(ReduceSpeedTimerHandle, this, &ATankPawn::ClearReduceSpeedTimer, MovementSoundReductionTime);

	if (!IsValid(MovementSoundComponent)) return;
	LastSoundVolume = MovementSoundComponent->VolumeMultiplier;
}

void ATankPawn::ScheduleCooldownResetOnNextTick()
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	World->GetTimerManager().SetTimerForNextTick(this, &ATankPawn::ResetCooldownWidget);
}

void ATankPawn::ClearReduceSpeedTimer()
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	World->GetTimerManager().ClearTimer(ReduceSpeedTimerHandle);
}

void ATankPawn::ReduceVolumeOverTime()
{
	if (!ReduceSpeedTimerHandle.IsValid() || MovementSoundReductionTime <= KINDA_SMALL_NUMBER) return;

	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	const float ElapsedTime = World->GetTimerManager().GetTimerElapsed(ReduceSpeedTimerHandle);
	const float NewVolume = UKismetMathLibrary::FInterpTo(LastSoundVolume, 0.f, ElapsedTime / MovementSoundReductionTime, 1.f);
	SetMovementSoundVolumeInComponent(NewVolume);
}

void ATankPawn::PlayOnFireCameraShake() const
{
	if (!IsValid(OnFireCameraShakeData.CameraShakeClass)) return;

	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	APlayerController* PlayerController = GetPlayerController();
	if (!IsValid(PlayerController)) return;

	PlayerController->ClientStartCameraShake(OnFireCameraShakeData.CameraShakeClass);
}

void ATankPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsLocallyControlled())
	{
		RotateTurretMeshByCursor(DeltaSeconds);
		RefreshCooldownWidget();
	}

	ReduceVolumeOverTime();
}

void ATankPawn::BeginPlay()
{
	Super::BeginPlay();

	ResetMomentSoundVolume();
	ActivateMovementSound();
	HideCursor();
	ScheduleCooldownResetOnNextTick();
}

void ATankPawn::SetActorTickEnabled(bool bEnabled)
{
	Super::SetActorTickEnabled(bEnabled);
	OnSetActorTickEnabled(bEnabled);
}

void ATankPawn::Destroyed()
{
	if (!IsValid(CameraComponent)) return Super::Destroyed();

	APlayerController* PlayerController = GetPlayerController();
	const FRotator Rotation = CameraComponent->GetComponentRotation();
	const FVector Location = CameraComponent->GetComponentLocation();

	Super::Destroyed();

	if (!IsValid(PlayerController) || !HasAuthority()) return;

	UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	ASpectatorPawn* NewActor = World->SpawnActor<ASpectatorPawn>(ASpectatorPawn::StaticClass(), Location, Rotation);
	if (!IsValid(NewActor)) return;

	PlayerController->Possess(NewActor);
}

void ATankPawn::OnSuccessfulFire() const
{
	Super::OnSuccessfulFire();

	if (IsValid(AmmoComponent))
	{
		AmmoComponent->Fire();
	}
}

bool ATankPawn::CanFire() const
{
	return IsValid(AmmoComponent) && AmmoComponent->CanShoot() && Super::CanFire();
}

void ATankPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATankPawn, AccelerationDurationElapsed);
	DOREPLIFETIME(ATankPawn, bIsMovingForward);
	DOREPLIFETIME(ATankPawn, LastDirectionChangedTime);
	DOREPLIFETIME(ATankPawn, LastSoundVolume);
	DOREPLIFETIME_CONDITION(ATankPawn, MovementSoundVolume, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ATankPawn, SmokeSpeed, COND_SkipOwner);
}

void ATankPawn::ClientPlayVFXOnFire_Implementation() const
{
	Super::ClientPlayVFXOnFire_Implementation();

	PlayOnFireCameraShake();
}

APlayerController* ATankPawn::GetPlayerController() const
{
	return Cast<APlayerController>(GetController());
}

void ATankPawn::SetSmokeSpeed(const float NewSpeed)
{
	SmokeSpeed = NewSpeed;
	OnRep_SmokeSpeed();
}

void ATankPawn::SetMovementSound(const float NewSoundVolume)
{
	MovementSoundVolume = NewSoundVolume;
	OnRep_MovementSoundVolume();
}