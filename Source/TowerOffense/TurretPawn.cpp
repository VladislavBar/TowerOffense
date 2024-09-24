#include "TurretPawn.h"

#include "Kismet/KismetMathLibrary.h"

ATurretPawn::ATurretPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	NewRootComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RootComponent"));
	RootComponent = NewRootComponent;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMesh->SetupAttachment(RootComponent);

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretMesh"));
	TurretMesh->SetupAttachment(RootComponent);

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSpawnPoint"));
	ProjectileSpawnPoint->SetupAttachment(TurretMesh);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
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

void ATurretPawn::Fire()
{
	if (!bCanFire || !IsValid(ProjectileClass) || !IsValid(ProjectileSpawnPoint)) return;

	UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;

	AProjectile* Projectile = World->SpawnActor<AProjectile>(ProjectileClass, ProjectileSpawnPoint->GetComponentTransform(), SpawnParameters);
	if (!IsValid(Projectile)) return;

	Projectile->SetProjectileSpeed(ProjectileSpeed);
	Projectile->SetDamage(Damage);

	DisableFire();
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

void ATurretPawn::StartCooldownTimer()
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	World->GetTimerManager().SetTimer(FireCooldownTimerHandle, this, &ATurretPawn::EnableFire, FireCooldown, false);
}

void ATurretPawn::DisableFire()
{
	bCanFire = false;
	StartCooldownTimer();
}

void ATurretPawn::EnableFire()
{
	bCanFire = true;
}

void ATurretPawn::SetupOnDeathDelegate()
{
	if (!IsValid(HealthComponent)) return;

	HealthComponent->OnDeath.AddUObject(this, &ATurretPawn::OnDeath);
}

void ATurretPawn::OnDeath()
{
	Destroy();
}

void ATurretPawn::RotateTurretMeshToLocation(const float DeltaSeconds, const FVector& Location, bool bInstantRotation)
{
	if (!IsValid(TurretMesh)) return;
	if (bInstantRotation) return RotateWithoutInterp(Location, DeltaSeconds);

	return RotateWithInterp(Location, DeltaSeconds);
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
	DrawDebugAtSpawnPointLocation();
}

void ATurretPawn::BeginPlay()
{
	Super::BeginPlay();
	SetupOnDeathDelegate();
}
