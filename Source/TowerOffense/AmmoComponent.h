#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AmmoComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAmmoInitializedDelegate, const int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAmmoChangedDelegate, const int32);

DECLARE_MULTICAST_DELEGATE(FOnAmmoReplenishStartsDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAmmoReplenishRemainingTimeDelegate, const float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAmmoReplenishFinishesDelegate, const float);

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
	int32 CurrentAmmo = 10;

	UPROPERTY(EditAnywhere, Category = "Ammo", meta = (ClampMin = "0"))
	float ReplenishTime = 5.0f;

	FTimerHandle ReplenishTimerHandle;

public:
	UAmmoComponent();

private:
	void ScheduleOnAmmoInitializedNotification() const;
	void ScheduleAmmoReplenishTimer();
	void NotifyAmmoInitialized() const;
	void ReplenishAmmo();
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
