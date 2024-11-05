#include "HealthComponent.h"

#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogHealthComponent)

UHealthComponent::UHealthComponent()
{
	SetIsReplicatedByDefault(true);
}

void UHealthComponent::TakeHit(float Damage)
{
	const AActor* Owner = GetOwner();
	if (!IsValid(Owner) && !Owner->HasAuthority()) return;

	ServerTakeHit(Damage);
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	SetupHealth();
}

void UHealthComponent::SetupHealth()
{
	SetHealthPoints(MaxHealth);
	if (HealthPoints <= UE_KINDA_SMALL_NUMBER)
	{
		UE_LOG(LogTemp, Error, TEXT("Health points are less than or equal to zero. Please set a valid value."));
		OnDeath.Broadcast();
	}
}

void UHealthComponent::OnRep_HealthPoints(float OldHealthPoints) const
{
	// if healed - ignore
	if (HealthPoints > OldHealthPoints) return;
	return OnDamageTakenBroadcastDelegates(OldHealthPoints - HealthPoints);
}

void UHealthComponent::ServerTakeHit_Implementation(float Damage)
{
	SetHealthPoints(HealthPoints - Damage);
	OnDamageTakenBroadcastDelegates(Damage);
}

void UHealthComponent::OnDamageTakenBroadcastDelegates(float Damage) const
{
	OnHitTaken.Broadcast(FHitTakenData{Damage, HealthPoints, MaxHealth});
	if (HealthPoints <= UE_KINDA_SMALL_NUMBER)
	{
		OnDeath.Broadcast();
	}
}

void UHealthComponent::SetHealthPoints(float NewHealthPoints)
{
	if (NewHealthPoints <= UE_KINDA_SMALL_NUMBER) NewHealthPoints = 0.f;
	HealthPoints = NewHealthPoints;
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UHealthComponent, HealthPoints);
}

FDelegateHandle UHealthComponent::AddOnHitTakenHandler(const FOnHitTakenDelegate::FDelegate& Delegate)
{
	return OnHitTaken.Add(Delegate);
}

FDelegateHandle UHealthComponent::AddOnDeathHandler(const FOnDeathDelegate::FDelegate& Delegate)
{
	return OnDeath.Add(Delegate);
}
