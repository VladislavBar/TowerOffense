#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

class UHealthWidget;

DECLARE_LOG_CATEGORY_EXTERN(LogHealthComponent, Log, All);

USTRUCT()
struct FHitTakenData
{
	GENERATED_BODY()

	UPROPERTY()
	float Damage = 0.f;

	UPROPERTY()
	float CurrentHealth = 0.f;

	UPROPERTY()
	float MaxHealth = 0.f;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnHitTakenDelegate, const FHitTakenData&);
DECLARE_MULTICAST_DELEGATE(FOnDeathDelegate);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TOWEROFFENSE_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

	FOnHitTakenDelegate OnHitTaken;
	FOnDeathDelegate OnDeath;

	UPROPERTY(EditAnywhere, Category = "Health", meta = (ClampMin = "0.0"))
	float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_HealthPoints)
	float HealthPoints = 100.f;

public:
	UHealthComponent();

private:
	virtual void BeginPlay() override;
	void SetupHealth();

	UFUNCTION()
	void OnRep_HealthPoints(float OldHealthPoints) const;

	UFUNCTION(Server, Reliable)
	void ServerTakeHit(float Damage);
	void OnDamageTakenBroadcastDelegates(float Damage) const;
	void SetHealthPoints(float NewHealthPoints);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	float GetHealthPoints() const { return HealthPoints; }
	float GetMaxHealth() const { return MaxHealth; }

	void TakeHit(float Damage);

	FDelegateHandle AddOnHitTakenHandler(const FOnHitTakenDelegate::FDelegate& Delegate);
	FDelegateHandle AddOnDeathHandler(const FOnDeathDelegate::FDelegate& Delegate);
};
