#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Projectile.generated.h"

UCLASS()
class TOWEROFFENSE_API AProjectile : public AActor
{
	GENERATED_BODY()

	friend class ATurretPawn;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	TObjectPtr<UStaticMeshComponent> ProjectileMesh;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	TObjectPtr<UNiagaraSystem> OnHitParticleEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float LifeSpan = 5.f;

	UPROPERTY()
	float Damage = 10.f;

public:
	AProjectile();

private:
	virtual void BeginPlay() override;
	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal,
		FVector NormalImpulse, const FHitResult& Hit) override;
	void SetupIgnoreActors();
	void DamageTarget(AActor* Target);
	void SetProjectileSpeed(float Speed);
	void SetDamage(float NewDamage);
	void EmitOnHitProjectileEffect(const FVector& Location, const FVector& Normal);
};
