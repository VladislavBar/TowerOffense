#include "Projectile.h"

#include "NiagaraEmitter.h"
#include "NiagaraFunctionLibrary.h"
#include "TurretPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	RootComponent = ProjectileMesh;

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
}

void AProjectile::SetProjectileSpeed(const float Speed)
{
	ProjectileMovementComponent->SetVelocityInLocalSpace(FVector(Speed, 0, 0));
}

void AProjectile::SetDamage(float NewDamage)
{
	Damage = NewDamage;
}

void AProjectile::EmitOnHitProjectileEffect(const FVector& Location, const FVector& Normal)
{
	if (!IsValid(OnHitParticleEffect)) return;

	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, OnHitParticleEffect, Location, Normal.Rotation());
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(LifeSpan);
	SetupIgnoreActors();
}

void AProjectile::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal,
	FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
	DamageTarget(Other);
	EmitOnHitProjectileEffect(HitLocation, HitNormal);
	Destroy();
}

void AProjectile::SetupIgnoreActors()
{
	if (!IsValid(RootComponent)) return;

	UStaticMeshComponent* Root = Cast<UStaticMeshComponent>(RootComponent);
	if (!IsValid(Root)) return;

	Root->IgnoreActorWhenMoving(Owner, true);
}

void AProjectile::DamageTarget(AActor* Target)
{
	ATurretPawn* TurretPawn = Cast<ATurretPawn>(Target);
	if (!IsValid(TurretPawn)) return;

	TurretPawn->TakeHit(Damage);
}