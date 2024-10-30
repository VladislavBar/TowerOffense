#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AmmoComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAmmoComponent, Log, All);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAmmoInitializedDelegate, const int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAmmoChangedDelegate, const int32);

DECLARE_MULTICAST_DELEGATE(FOnAmmoReplenishStartsDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAmmoReplenishRemainingTimeDelegate, const float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAmmoReplenishFinishesDelegate, const int32);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TOWEROFFENSE_API UAmmoComponent : public UActorComponent
{
	GENERATED_BODY()

	FOnAmmoInitializedDelegate OnAmmoInitializedDelegate;
	FOnAmmoChangedDelegate OnAmmoChangedDelegate;
	FOnAmmoReplenishStartsDelegate OnAmmoReplenishStarts;
	FOnAmmoReplenishRemainingTimeDelegate OnAmmoReplenishRemainingTime;
	FOnAmmoReplenishFinishesDelegate OnAmmoReplenishFinishes;

	UPROPERTY(EditAnywhere, Category = "Ammo", meta = (ClampMin = "0"))
	int32 MaxAmmo = 10;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentAmmo)
	int32 CurrentAmmo = 10;

	UPROPERTY(EditAnywhere, Category = "Ammo", meta = (ClampMin = "0"))
	float ReplenishTime = 5.0f;

	FTimerHandle ReplenishTimerHandle;

public:
	UAmmoComponent();

private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void ScheduleOnAmmoInitializedNotification() const;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerScheduleAmmoReplenishTimer();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastScheduleAmmoReplenishTimer();
	
	void NotifyAmmoInitialized() const;
	void ReplenishAmmo();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastReplenishAmmo();

	UFUNCTION()
	void OnRep_CurrentAmmo() const;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

public:
	FDelegateHandle AddOnAmmoInitializedHandler(const FOnAmmoInitializedDelegate::FDelegate& Delegate);
	FDelegateHandle AddOnAmmoChangedHandler(const FOnAmmoChangedDelegate::FDelegate& Delegate);
	FDelegateHandle AddAmmoReplenishStarts(const FOnAmmoReplenishStartsDelegate::FDelegate& Delegate);
	FDelegateHandle AddAmmoReplenishRemainingTime(const FOnAmmoReplenishRemainingTimeDelegate::FDelegate& Delegate);
	FDelegateHandle AddAmmoReplenishFinishes(const FOnAmmoReplenishFinishesDelegate::FDelegate& Delegate);

	void Fire();
	bool CanShoot() const;
};
