#include "Projectile.h"

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

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(LifeSpan);
	SetupIgnoreActors();
}

void AProjectile::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	Destroy();
}

void AProjectile::SetupIgnoreActors()
{
	if (!IsValid(RootComponent)) return;

	UStaticMeshComponent* Root = Cast<UStaticMeshComponent>(RootComponent);
	if (!IsValid(Root)) return;

	Root->IgnoreActorWhenMoving(Owner, true);
}