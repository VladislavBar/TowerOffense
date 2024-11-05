#include "TurretPawn.h"

#include "NiagaraComponent.h"
#include "TankPawn.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"

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
	EmitOnDeathEffect();
	Destroy();
}

void ATurretPawn::EmitOnDeathEffect() const
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

void ATurretPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATurretPawn, bCanFire);
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

void ATurretPawn::SetupTeamColorDynamicMaterial(UStaticMeshComponent* Mesh)
{
	if (!IsValid(Mesh) || MaterialTeamColorSlotName.IsNone()) return;

	const int32 MaterialIndex = Mesh->GetMaterialIndex(MaterialTeamColorSlotName);
	if (MaterialIndex == INDEX_NONE) return;

	UMaterialInterface* Material = Mesh->GetMaterial(MaterialIndex);
	if (!IsValid(Material)) return;

	UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(Material, nullptr);
	DynamicMaterial->SetVectorParameterValue(MaterialTeamColorParameterName, TeamColor);

	Mesh->SetMaterial(MaterialIndex, DynamicMaterial);
}

void ATurretPawn::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	SetupTeamColorDynamicMaterial(BaseMesh);
	SetupTeamColorDynamicMaterial(TurretMesh);
}

void ATurretPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	RotateTurretMesh(DeltaSeconds);
}

void ATurretPawn::BeginPlay()
{
	Super::BeginPlay();
	SetupOnDeathDelegate();
	AdjustRotationSoundVolume(0.f);
	EnableRotationSound();
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
