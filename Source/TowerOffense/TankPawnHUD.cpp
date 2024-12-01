#include "TankPawnHUD.h"

DEFINE_LOG_CATEGORY(LogTankPawnHUD);

void UTankPawnHUD::NativeConstruct()
{
	Super::NativeConstruct();

	OnMatchWaitingToStart();
	SetupDelegates();

	ResetAmmoWidgetHideTimer();
	ResetHealthBarHideTimer();
}

void UTankPawnHUD::ResetHealthBarHideTimer()
{
	if (HealthBarHideTime <= KINDA_SMALL_NUMBER)
	{
		UE_LOG(LogTankPawnHUD, Warning, TEXT("HealthBarHideTime is too small!"));
		return;
	}

	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	if (HealthBarHideTimerHandle.IsValid() && World->GetTimerManager().IsTimerActive(HealthBarHideTimerHandle))
	{
		World->GetTimerManager().ClearTimer(HealthBarHideTimerHandle);
	}

	World->GetTimerManager().SetTimer(HealthBarHideTimerHandle, this, &UTankPawnHUD::HideHealthBar, HealthBarHideTime, false);
}

void UTankPawnHUD::ResetAmmoWidgetHideTimer()
{
	if (AmmoWidgetHideTime <= KINDA_SMALL_NUMBER)
	{
		UE_LOG(LogTankPawnHUD, Warning, TEXT("AmmoWidgetHideTime is too small!"));
		return;
	}

	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	if (AmmoWidgetHideTimerHandle.IsValid() && World->GetTimerManager().IsTimerActive(AmmoWidgetHideTimerHandle))
	{
		World->GetTimerManager().ClearTimer(AmmoWidgetHideTimerHandle);
	}

	World->GetTimerManager().SetTimer(AmmoWidgetHideTimerHandle, this, &UTankPawnHUD::HideAmmoWidget, AmmoWidgetHideTime, false);
}

void UTankPawnHUD::ClearAmmoWidgetHideTimer()
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	World->GetTimerManager().ClearTimer(AmmoWidgetHideTimerHandle);
}

void UTankPawnHUD::OnMatchWaitingToStart() const
{
	if (IsValid(EnemiesLeftWidget))
	{
		EnemiesLeftWidget->SetVisibility(ESlateVisibility::Hidden);
	}

	if (IsValid(EnemiesLeftWidget))
	{
		DelayedStartWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void UTankPawnHUD::OnMatchStarted() const
{
	if (IsValid(EnemiesLeftWidget))
	{
		EnemiesLeftWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

	if (IsValid(EnemiesLeftWidget))
	{
		DelayedStartWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UTankPawnHUD::SetupDelegates()
{
	SetupAmmoDelegates();
	SetupHealthDelegates();
}

void UTankPawnHUD::SetupAmmoDelegates()
{
	const APawn* OwnerPawn = GetOwningPlayerPawn();
	if (!IsValid(OwnerPawn)) return;

	UAmmoComponent* AmmoComponent = OwnerPawn->FindComponentByClass<UAmmoComponent>();
	if (!IsValid(AmmoComponent)) return;

	OnAmmoChangedDelegate.BindUObject(this, &UTankPawnHUD::OnAmmoChanged);
	OnReplenishStartsDelegate.BindUObject(this, &UTankPawnHUD::OnReplenishStarts);
	OnReplenishFinishesDelegate.BindUObject(this, &UTankPawnHUD::OnReplenishFinishes);

	OnAmmoChangedDelegateHandle = AmmoComponent->AddOnAmmoChangedHandler(OnAmmoChangedDelegate);
	OnReplenishStartsDelegateHandle = AmmoComponent->AddAmmoReplenishStarts(OnReplenishStartsDelegate);
	OnReplenishFinishesDelegateHandle = AmmoComponent->AddAmmoReplenishFinishes(OnReplenishFinishesDelegate);
}

void UTankPawnHUD::SetupHealthDelegates()
{
	const APawn* OwnerPawn = GetOwningPlayerPawn();
	if (!IsValid(OwnerPawn)) return;

	UHealthComponent* HealthComponent = OwnerPawn->FindComponentByClass<UHealthComponent>();
	if (!IsValid(HealthComponent)) return;

	OnHitTakenDelegate.BindUObject(this, &UTankPawnHUD::OnHitTaken);
	OnHitTakenDelegateHandle = HealthComponent->AddOnHitTakenHandler(OnHitTakenDelegate);
}

void UTankPawnHUD::SetupOnStartDelayDelegate()
{
	OnStartDelayDelegate.BindUObject(this, &UTankPawnHUD::HideCooldownWidget);
}

void UTankPawnHUD::OnAmmoChanged(const int32 Ammo)
{
	ShowComponent(&UTankPawnHUD::AmmoWidget);
	ResetAmmoWidgetHideTimer();
}

void UTankPawnHUD::OnReplenishStarts()
{
	ShowComponent(&UTankPawnHUD::AmmoWidget);
	ClearAmmoWidgetHideTimer();
}

void UTankPawnHUD::OnReplenishFinishes(const int32 Ammo)
{
	ShowComponent(&UTankPawnHUD::AmmoWidget);
	ResetAmmoWidgetHideTimer();
}

void UTankPawnHUD::OnHitTaken(const FHitTakenData& HitTakenData)
{
	ShowComponent(&UTankPawnHUD::HealthWidget);
	ResetHealthBarHideTimer();
}