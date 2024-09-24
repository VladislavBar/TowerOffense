#include "HealthComponent.h"

DEFINE_LOG_CATEGORY(LogHealthComponent)

void UHealthComponent::TakeHit(float Damage)
{
	HealthPoints -= Damage;

	OnHitTaken.Broadcast(FHitTakenData{Damage, HealthPoints, MaxHealth});
	if (HealthPoints <= UE_KINDA_SMALL_NUMBER)
	{
		OnDeath.Broadcast();
	}
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	SetupHealth();
}

void UHealthComponent::SetupHealth()
{
	HealthPoints = MaxHealth;
	if (HealthPoints <= UE_KINDA_SMALL_NUMBER)
	{
		UE_LOG(LogTemp, Error, TEXT("Health points are less than or equal to zero. Please set a valid value."));
		OnDeath.Broadcast();
	}
}