#include "AmmoComponent.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogAmmoComponent);

UAmmoComponent::UAmmoComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UAmmoComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAmmoComponent, CurrentAmmo);
}


void UAmmoComponent::Fire()
{
	--CurrentAmmo;

	OnAmmoChangedDelegate.Broadcast(CurrentAmmo);
	if (CurrentAmmo <= 0)
	{
		ServerScheduleAmmoReplenishTimer();
	}
}

bool UAmmoComponent::CanShoot() const
{
	return CurrentAmmo > 0;
}

void UAmmoComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentAmmo = MaxAmmo;
	ScheduleOnAmmoInitializedNotification();
}

void UAmmoComponent::ScheduleOnAmmoInitializedNotification() const
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	World->GetTimerManager().SetTimerForNextTick(this, &UAmmoComponent::NotifyAmmoInitialized);
}

bool UAmmoComponent::ServerScheduleAmmoReplenishTimer_Validate()
{
	const AActor* Owner = GetOwner();
	if (!IsValid(Owner)) return false;

	return Owner->HasAuthority();
}

void UAmmoComponent::ServerScheduleAmmoReplenishTimer_Implementation()
{
	if (ReplenishTime <= KINDA_SMALL_NUMBER)
	{
		UE_LOG(LogAmmoComponent, Warning, TEXT("ReplenishTime is too small!"));
		return;
	}

	MulticastScheduleAmmoReplenishTimer();
}

void UAmmoComponent::MulticastScheduleAmmoReplenishTimer_Implementation()
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	const bool bIsActiveTimer = World->GetTimerManager().IsTimerActive(ReplenishTimerHandle);
	if (bIsActiveTimer) return;

	World->GetTimerManager().SetTimer(ReplenishTimerHandle, this, &UAmmoComponent::ReplenishAmmo, ReplenishTime);
	OnAmmoReplenishStarts.Broadcast();
}

void UAmmoComponent::NotifyAmmoInitialized() const
{
	OnAmmoInitializedDelegate.Broadcast(CurrentAmmo);
}

void UAmmoComponent::ReplenishAmmo()
{
	const AActor* Owner = GetOwner();
	if (!IsValid(Owner) || !Owner->HasAuthority()) return;

	CurrentAmmo = MaxAmmo;
	MulticastReplenishAmmo();
}

void UAmmoComponent::MulticastReplenishAmmo_Implementation()
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	World->GetTimerManager().ClearTimer(ReplenishTimerHandle);

	OnAmmoReplenishFinishes.Broadcast(CurrentAmmo);
	OnAmmoChangedDelegate.Broadcast(CurrentAmmo);
}

void UAmmoComponent::OnRep_CurrentAmmo() const
{
	OnAmmoChangedDelegate.Broadcast(CurrentAmmo);
}

void UAmmoComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!ReplenishTimerHandle.IsValid()) return;

	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	const bool bIsActiveTimer = World->GetTimerManager().IsTimerActive(ReplenishTimerHandle);
	if (!bIsActiveTimer) return;

	const float RemainingTime = World->GetTimerManager().GetTimerRemaining(ReplenishTimerHandle);
	OnAmmoReplenishRemainingTime.Broadcast(RemainingTime);
}

FDelegateHandle UAmmoComponent::AddOnAmmoInitializedHandler(const FOnAmmoInitializedDelegate::FDelegate& Delegate)
{
	return OnAmmoInitializedDelegate.Add(Delegate);
}

FDelegateHandle UAmmoComponent::AddOnAmmoChangedHandler(const FOnAmmoChangedDelegate::FDelegate& Delegate)
{
	return OnAmmoChangedDelegate.Add(Delegate);
}

FDelegateHandle UAmmoComponent::AddAmmoReplenishStarts(const FOnAmmoReplenishStartsDelegate::FDelegate& Delegate)
{
	return OnAmmoReplenishStarts.Add(Delegate);
}

FDelegateHandle UAmmoComponent::AddAmmoReplenishRemainingTime(const FOnAmmoReplenishRemainingTimeDelegate::FDelegate& Delegate)
{
	return OnAmmoReplenishRemainingTime.Add(Delegate);
}

FDelegateHandle UAmmoComponent::AddAmmoReplenishFinishes(const FOnAmmoReplenishFinishesDelegate::FDelegate& Delegate)
{
	return OnAmmoReplenishFinishes.Add(Delegate);
}
