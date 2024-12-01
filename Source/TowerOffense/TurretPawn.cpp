#include "TurretPawn.h"

#include "HealthBarWidgetComponent.h"
#include "NiagaraComponent.h"
#include "TankPawn.h"
#include "TeamData.h"
#include "TeamHelper.h"
#include "TowerOffenseGameState.h"
#include "TowerOffensePlayerState.h"
#include "Components/AudioComponent.h"
#include "GameFramework/GameState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"

DEFINE_LOG_CATEGORY(LogTurretPawn);

ATurretPawn::ATurretPawn()
{
	bReplicates = true;
	SetReplicateMovement(true);

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	NewRootComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RootComponent"));
	RootComponent = NewRootComponent;
	RootComponent->SetIsReplicated(true);

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMesh->SetupAttachment(RootComponent);
	BaseMesh->SetIsReplicated(true);

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretMesh"));
	TurretMesh->SetupAttachment(RootComponent);
	TurretMesh->SetIsReplicated(true);

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSpawnPoint"));
	ProjectileSpawnPoint->SetupAttachment(TurretMesh);
	ProjectileSpawnPoint->SetIsReplicated(true);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	OnFireEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("OnFireEffectComponent"));
	OnFireEffectComponent->SetupAttachment(TurretMesh);

	OnRotationSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("OnRotationSoundComponent"));
	OnRotationSoundComponent->SetupAttachment(TurretMesh);
	OnRotationSoundComponent->bAutoActivate = false;
	OnRotationSoundComponent->Stop();

	HealthBarWidgetComponent = CreateDefaultSubobject<UHealthBarWidgetComponent>(TEXT("HealthBarWidgetComponent"));
	HealthBarWidgetComponent->SetupAttachment(RootComponent);
}

TArray<FName> ATurretPawn::GetMaterialTeamColorSlotNames() const
{
	TSet<FName> MaterialNames;

	if (IsValid(BaseMesh))
	{
		MaterialNames.Append(BaseMesh->GetMaterialSlotNames());
	}

	if (IsValid(TurretMesh))
	{
		MaterialNames.Append(TurretMesh->GetMaterialSlotNames());
	}

	return MaterialNames.Array();
}

void ATurretPawn::RotateTurretMesh(const float DeltaSeconds)
{
	if (!bLockTarget || !IsValid(TurretMesh)) return;
	return RotateTurretMeshToLocation(DeltaSeconds, TargetLocation);
}

void ATurretPawn::DrawDebugAtSpawnPointLocation() const
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	DrawDebugSphere(World, GetProjectileSpawnLocation(), ProjectileDebugSphereRadius, ProjectileDebugSphereSegments, ProjectileDebugSphereColor);
}

bool ATurretPawn::CanFire() const
{
	return bCanFire && IsValid(ProjectileClass) && IsValid(ProjectileSpawnPoint);
}

void ATurretPawn::OnSuccessfulFire() const
{
	ClientPlayVFXOnFire();
	MulticastPlayVFXOnFire();
}

void ATurretPawn::ClientPlayVFXOnFire_Implementation() const {}

void ATurretPawn::ServerFire_Implementation()
{
	if (!CanFire()) return;

	UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;

	AProjectile* Projectile = World->SpawnActor<AProjectile>(ProjectileClass, ProjectileSpawnPoint->GetComponentTransform(), SpawnParameters);
	if (!IsValid(Projectile)) return;

	Projectile->SetProjectileSpeed(ProjectileSpeed);
	Projectile->SetDamage(Damage);

	DisableFire();
	OnSuccessfulFire();
}

bool ATurretPawn::ServerFire_Validate()
{
	return CanFire();
}

void ATurretPawn::MulticastPlayVFXOnFire_Implementation() const
{
	if (IsValid(OnFireEffectComponent))
	{
		OnFireEffectComponent->ActivateSystem();
	}
}

void ATurretPawn::ClientFire_Implementation()
{
	if (!CanFire()) return;

	if (!IsLocallyControlled() && !HasAuthority()) return;
	ServerFire();
}

void ATurretPawn::RotateWithoutInterp(const FVector& CurrentTargetLocation, const float DeltaSeconds)
{
	if (!IsValid(TurretMesh)) return;

	const FRotator CurrentRotation = TurretMesh->GetComponentRotation();
	const FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CurrentTargetLocation);
	const float DeltaRotationYaw = FRotator::NormalizeAxis(TargetRotation.Yaw - CurrentRotation.Yaw);
	const float YawOffset = DeltaRotationYaw * DeltaSeconds * RotationSpeedWhenTargetLocked;
	const float YawOffsetClamped = FMath::Clamp(YawOffset, -MaxInstantRotationSpeed, MaxInstantRotationSpeed);
	const FRotator RotationOffset = FRotator(0, YawOffsetClamped, 0);

	TurretMesh->AddWorldRotation(RotationOffset);
	SetSpawnPointRotationAtLocation(CurrentTargetLocation);
}

void ATurretPawn::RotateWithInterp(const FVector& CurrentTargetLocation, const float DeltaSeconds)
{
	if (!IsValid(TurretMesh)) return;

	const FRotator CurrentRotation = TurretMesh->GetComponentRotation();
	const FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CurrentTargetLocation);

	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaSeconds, RotationInterpExponent);
	NewRotation.Roll = 0.f;
	NewRotation.Pitch = 0.f;

	TurretMesh->SetWorldRotation(NewRotation);
	SetSpawnPointRotationAtLocation(CurrentTargetLocation);
}

void ATurretPawn::SetSpawnPointRotationAtLocation(const FVector& CurrentTargetLocation)
{
	if (!IsValid(ProjectileSpawnPoint)) return;

	const FRotator CurrentRotation = ProjectileSpawnPoint->GetComponentRotation();
	FRotator NewBulletSpawnTargetRotation = UKismetMathLibrary::FindLookAtRotation(GetProjectileSpawnLocation(), CurrentTargetLocation);
	NewBulletSpawnTargetRotation.Roll = CurrentRotation.Roll;
	NewBulletSpawnTargetRotation.Yaw = CurrentRotation.Yaw;

	ProjectileSpawnPoint->SetWorldRotation(NewBulletSpawnTargetRotation);
}

void ATurretPawn::MulticastStartCooldownTimer_Implementation()
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	World->GetTimerManager().SetTimer(FireCooldownTimerHandle, this, &ATurretPawn::EnableFire, FireCooldown, false);
}

void ATurretPawn::DisableFire()
{
	bCanFire = false;
	MulticastStartCooldownTimer();
}

void ATurretPawn::EnableFire()
{
	bCanFire = true;

	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	World->GetTimerManager().ClearTimer(FireCooldownTimerHandle);
}

void ATurretPawn::SetupOnDeathDelegate()
{
	if (!IsValid(HealthComponent)) return;

	OnDeathDelegate.BindUObject(this, &ATurretPawn::OnDeath);
	OnDeathDelegateHandle = HealthComponent->AddOnDeathHandler(OnDeathDelegate);
}

void ATurretPawn::OnDeath()
{
	MulticastEmitOnDeathEffect();
	Destroy();
}

void ATurretPawn::MulticastEmitOnDeathEffect_Implementation() const
{
	EmitOnDeathSFX();
	EmitOnDeathVFX();
	EmitOnDeathCameraShake();
}

void ATurretPawn::EmitOnDeathVFX() const
{
	if (!IsValid(OnDeathEffect)) return;

	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	UGameplayStatics::SpawnEmitterAtLocation(World, OnDeathEffect, GetActorLocation(), FRotator::ZeroRotator);
}

void ATurretPawn::EmitOnDeathCameraShake() const
{
	if (!IsValid(OnDeathCameraShakeData.CameraShakeClass)) return;

	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	UGameplayStatics::PlayWorldCameraShake(World, OnDeathCameraShakeData.CameraShakeClass, GetActorLocation(), OnDeathCameraShakeData.InnerRadius,
		OnDeathCameraShakeData.OuterRadius, OnDeathCameraShakeData.Falloff);
}

void ATurretPawn::EmitOnDeathSFX() const
{
	if (!IsValid(OnDeathSound)) return;

	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	UGameplayStatics::PlaySoundAtLocation(World, OnDeathSound, GetActorLocation());
}

void ATurretPawn::EnableRotationSound()
{
	if (!IsValid(OnRotationSoundComponent)) return;

	OnRotationSoundComponent->Activate();
	OnRotationSoundComponent->Play();
}

void ATurretPawn::AdjustRotationSoundVolume(const float RotationDifference)
{
	if (!IsValid(OnRotationSoundComponent)) return;

	const float Volume = UKismetMathLibrary::FClamp(RotationDifference * RotationSoundVolumeMultiplier, 0.f, 1.f);
	OnRotationSoundComponent->SetVolumeMultiplier(Volume);
}

void ATurretPawn::SetActorTickEnabled(bool bEnabled)
{
	Super::SetActorTickEnabled(bEnabled);

	OnActorTickableEnabledDelegate.Broadcast(bEnabled);
}

void ATurretPawn::MulticastSetActorTickEnabled_Implementation(const bool bEnabled)
{
	SetActorTickEnabled(bEnabled);
}

void ATurretPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATurretPawn, bCanFire);
	DOREPLIFETIME(ATurretPawn, CurrentTeam);
	DOREPLIFETIME(ATurretPawn, bIsInitialized);
}

void ATurretPawn::CommandLineSetTeam(const ETeam NewTeam)
{
	ServerSetTeam(NewTeam);
}

void ATurretPawn::ServerSetTeam_Implementation(const ETeam NewTeam)
{
	SetTeam(NewTeam);
	OnRep_CurrentTeam();
}

void ATurretPawn::SetTeam(const ETeam NewTeam)
{
	CurrentTeam = NewTeam;
	SyncPlayerStateTeam();
}

void ATurretPawn::SyncPlayerStateTeam_Implementation() const
{
	ATowerOffensePlayerState* CurrentPlayerState = Cast<ATowerOffensePlayerState>(GetPlayerState());
	if (!IsValid(CurrentPlayerState)) return;

	CurrentPlayerState->SetTeam(CurrentTeam);
}

void ATurretPawn::RotateTurretMeshToLocation(const float DeltaSeconds, const FVector& Location, bool bInstantRotation)
{
	if (IsLocallyControlled())
	{
		ServerRotateTurretMeshToLocation(DeltaSeconds, Location, bInstantRotation);
		ClientRotateTurretMeshToLocation(DeltaSeconds, Location, bInstantRotation);
		return;
	}

	if (HasAuthority() && GetRemoteRole() == ROLE_SimulatedProxy)
	{
		ServerRotateTurretMeshToLocation(DeltaSeconds, Location, bInstantRotation);
		return;
	}
}

FDelegateHandle ATurretPawn::AddOnActorTickableEnabledHandler(const FOnActorTickableEnabledDelegate::FDelegate& Delegate)
{
	return OnActorTickableEnabledDelegate.Add(Delegate);
}

void ATurretPawn::RemoveOnActorTickableEnabledHandler(const FDelegateHandle& Handle)
{
	OnActorTickableEnabledDelegate.Remove(Handle);
}

FDelegateHandle ATurretPawn::AddOnTeamChangedHandler(const FOnTeamChangedDelegate::FDelegate& Delegate)
{
	return OnTeamChangedDelegate.Add(Delegate);
}

void ATurretPawn::RemoveOnTeamChangedHandler(const FDelegateHandle& Handle)
{
	OnTeamChangedDelegate.Remove(Handle);
}

void ATurretPawn::RotateTurretMeshToLocation_Internal(const float DeltaSeconds, const FVector& Location, bool bInstantRotation)
{
	if (!IsValid(TurretMesh)) return;

	if (bInstantRotation)
	{
		RotateWithoutInterp(Location, DeltaSeconds);
	}
	else
	{
		RotateWithInterp(Location, DeltaSeconds);
	}
}

void ATurretPawn::ClientRotateTurretMeshToLocation_Implementation(const float DeltaSeconds, const FVector& Location, bool bInstantRotation)
{
	const FRotator PreviousRotation = GetTurretMeshRotation();
	RotateTurretMeshToLocation_Internal(DeltaSeconds, Location, bInstantRotation);
	PlaySoundOnRotation(PreviousRotation);
}

void ATurretPawn::PlaySoundOnRotation(const FRotator& PreviousRotation)
{
	const FRotator NewRotation = TurretMesh->GetComponentRotation();
	const float RotationDifference = UKismetMathLibrary::Abs(NewRotation.Yaw - PreviousRotation.Yaw);
	AdjustRotationSoundVolume(RotationDifference);
}

void ATurretPawn::OnRep_bCanFire()
{
	if (bCanFire)
	{
		const UWorld* World = GetWorld();
		if (!IsValid(World)) return;

		World->GetTimerManager().ClearTimer(FireCooldownTimerHandle);
	}
}

void ATurretPawn::OnRep_CurrentTeam()
{
	UpdateMaterialTeamColor();
	OnTeamChangedDelegate.Broadcast(this, CurrentTeam);
	SyncPlayerStateTeam();
}

void ATurretPawn::UpdateMaterialTeamColor()
{
	SetupTeamColorDynamicMaterial(BaseMesh);
	SetupTeamColorDynamicMaterial(TurretMesh);
}

void ATurretPawn::ServerFinishInitialization_Implementation()
{
	bIsInitialized = true;
	OnRep_bIsInitialized();
}

void ATurretPawn::ServerRotateTurretMeshToLocation_Implementation(const float DeltaSeconds, const FVector& Location, bool bInstantRotation)
{
	RotateTurretMeshToLocation_Internal(DeltaSeconds, Location, bInstantRotation);
}

void ATurretPawn::TakeHit(float DamageAmount)
{
	HealthComponent->TakeHit(DamageAmount);
}

FRotator ATurretPawn::GetTurretMeshRotation() const
{
	if (!IsValid(TurretMesh)) return FRotator::ZeroRotator;
	return TurretMesh->GetComponentRotation();
}

FVector ATurretPawn::GetProjectileSpawnLocation() const
{
	if (!IsValid(ProjectileSpawnPoint)) return FVector::ZeroVector;
	return ProjectileSpawnPoint->GetComponentLocation();
}

void ATurretPawn::SetTargetLocation(const FVector& Location)
{
	TargetLocation = Location;
}

void ATurretPawn::OnRep_bIsInitialized()
{
	if (!IsLocallyControlled()) return;

	ServerSyncUserState();
	UpdateMaterialTeamColor();
}

void ATurretPawn::ServerSyncUserState_Implementation()
{
	if (!bIsInitialized) return;

	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	const ATowerOffenseGameState* GameState = World->GetGameStateChecked<ATowerOffenseGameState>();
	if (!IsValid(GameState)) return;

	GameState->ServerSyncUserState(this);
}

void ATurretPawn::SetupTeamColorDynamicMaterial(UStaticMeshComponent* Mesh)
{
	if (!IsValid(Mesh) || MaterialTeamColorSlotName.IsNone()) return;

	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	const int32 MaterialIndex = Mesh->GetMaterialIndex(MaterialTeamColorSlotName);
	if (MaterialIndex == INDEX_NONE) return;

	UMaterialInterface* Material = Mesh->GetMaterial(MaterialIndex);
	if (!IsValid(Material)) return;

	if (!IsValid(TeamColorDynamicMaterial))
	{
		TeamColorDynamicMaterial = UMaterialInstanceDynamic::Create(Material, nullptr);
	}

	TeamColor = GetTeamColor();
	TeamColorDynamicMaterial->SetVectorParameterValue(MaterialTeamColorParameterName, TeamColor);

	Mesh->SetMaterial(MaterialIndex, TeamColorDynamicMaterial);
}

void ATurretPawn::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	UpdateMaterialTeamColor();
}

void ATurretPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	RotateTurretMesh(DeltaSeconds);
	RefreshHealthBarVisibility();
}

void ATurretPawn::BeginPlay()
{
	Super::BeginPlay();
	SetupOnDeathDelegate();
	AdjustRotationSoundVolume(0.f);
	EnableRotationSound();
	RefreshHealthBarVisibility();

	if (IsLocallyControlled() || (HasAuthority() && GetRemoteRole() == ROLE_SimulatedProxy))
	{
		ServerFinishInitialization();
	}
}

bool ATurretPawn::CanReach(const AActor* Target, const float MaxDistance) const
{
	if (FVector::DistXY(GetActorLocation(), Target->GetActorLocation()) > MaxDistance) return false;
	if (HasStraightView(Target)) return true;

	return false;
}

bool ATurretPawn::HasStraightView(const AActor* Target) const
{
	if (!IsValid(Target)) return false;

	const UWorld* World = GetWorld();
	if (!IsValid(World)) return false;

	FRotator ExpectedRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target->GetActorLocation());
	const FRotator CurrentRotation = GetRelativeTurretMeshRotation();
	const FVector ExpectedProjectileSpawnLocation =
		GetActorLocation() + (ExpectedRotation - CurrentRotation).RotateVector(GetRelativeProjectileSpawnLocation());

	FHitResult HitResult;
	UKismetSystemLibrary::LineTraceSingle(World, ExpectedProjectileSpawnLocation, Target->GetActorLocation(), UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false, TArray<AActor*>(), EDrawDebugTrace::None, HitResult, true, FLinearColor::Green, FLinearColor::Red, 1.f);
	return HitResult.GetActor() == Target;
}

void ATurretPawn::RotateTurretByYaw(const float Yaw) const
{
	if (!IsValid(TurretMesh)) return;

	const FRotator CurrentRotation = TurretMesh->GetComponentRotation();
	FRotator NewRotation = CurrentRotation;
	NewRotation.Yaw += Yaw;

	TurretMesh->SetWorldRotation(NewRotation);
}

FVector ATurretPawn::PredictTargetLocation(const ATankPawn* Target, float StartPredictingLocationAtAccelerationProgress) const
{
	if (!IsValid(Target) || GetProjectileSpeed() <= KINDA_SMALL_NUMBER) return FVector::ZeroVector;

	const FVector CurrentTargetLocation = Target->GetActorLocation();
	const FVector TargetForwardVector = Target->GetActorForwardVector();
	const float TimeToReachTarget = FVector::Distance(CurrentTargetLocation, GetActorLocation()) / GetProjectileSpeed();

	const float AccelerationElapsedTime = Target->GetElapsedTimeSinceLastDirectionChange();
	const float AccelerationDuration = Target->GetAccelerationDuration();
	if (AccelerationElapsedTime <= KINDA_SMALL_NUMBER || AccelerationElapsedTime <= AccelerationDuration * StartPredictingLocationAtAccelerationProgress)
	{
		return CurrentTargetLocation;
	}

	const float Direction = Target->IsMovingForward() ? 1.f : -1.f;
	return CurrentTargetLocation + TargetForwardVector * Target->GetSpeed() * TimeToReachTarget * Direction;
}

FVector ATurretPawn::GetRelativeProjectileSpawnLocation() const
{
	if (!IsValid(ProjectileSpawnPoint) || !IsValid(TurretMesh)) return FVector::ZeroVector;

	const FRotator TurretMeshRotation = TurretMesh->GetRelativeRotation();
	return TurretMeshRotation.RotateVector(ProjectileSpawnPoint->GetRelativeLocation());
}

FRotator ATurretPawn::GetRelativeTurretMeshRotation() const
{
	if (!IsValid(TurretMesh)) return FRotator::ZeroRotator;

	return TurretMesh->GetRelativeRotation();
}

void ATurretPawn::RefreshHealthBarVisibility() const
{
	if (!IsValid(HealthBarWidgetComponent)) return;

	const bool bIsVisibleToPlayer = IsVisibleToPlayer();
	if (HealthBarWidgetComponent->IsVisible() != bIsVisibleToPlayer)
	{
		HealthBarWidgetComponent->SetVisibility(bIsVisibleToPlayer);
	}
}

bool ATurretPawn::IsVisibleToPlayer() const
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return false;

	const APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!IsValid(PlayerController)) return false;

	const APawn* PlayerPawn = PlayerController->GetPawn();
	if (!IsValid(PlayerPawn) || PlayerPawn == this) return false;

	const ATowerOffenseGameState* GameState = Cast<ATowerOffenseGameState>(World->GetGameState());
	if (!IsValid(GameState)) return false;

	const TArray<AActor*> ActiveParticipants{GameState->GetActiveParticipants()};
	const TArray<AActor*> IgnoredActors = ActiveParticipants.FilterByPredicate([this, PlayerPawn](AActor* Actor) { return Actor != PlayerPawn; });

	FHitResult HitResult;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActors(IgnoredActors);
	World->LineTraceSingleByChannel(HitResult, GetActorLocation(), PlayerPawn->GetActorLocation(), ECC_Visibility, CollisionQueryParams);

	return HitResult.GetActor() == PlayerPawn;
}

FColor ATurretPawn::GetTeamColor() const
{
	if (!IsValid(TeamColorTable))
	{
		UE_LOG(LogTurretPawn, Error, TEXT("TeamColorTable is not set"));
		return FColor::Black;
	}

	TArray<FTeamData*> TeamColorRows;
	TeamColorTable->GetAllRows<FTeamData>("", TeamColorRows);

	for (const FTeamData* TeamData : TeamColorRows)
	{
		if (TeamData->Team == CurrentTeam) return TeamData->Color.ToFColor(true);
	}

	UE_LOG(LogTurretPawn, Error, TEXT("Team color for team %d is not set"), static_cast<int32>(CurrentTeam));
	return FColor::Black;
}
