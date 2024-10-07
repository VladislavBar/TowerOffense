#include "AmmoWidget.h"

void UAmmoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetupDelegates();
}

void UAmmoWidget::SetAmmoText(const int32 Ammo)
{
	if (!IsValid(AmmoText)) return;

	const FText AmmoMessage = GetAmmoText(Ammo);
	AmmoText->SetText(AmmoMessage);
}

FText UAmmoWidget::GetAmmoText(const int32 Ammo) const
{
	if (Ammo <= 0) return FText::FromString("Out of ammo!");

	const FString AmmoMessage = FString::Printf(TEXT("Ammo: %d"), Ammo);
	return FText::FromString(AmmoMessage);
}

void UAmmoWidget::SetupDelegates()
{
	const APawn* OwnerPawn = GetOwningPlayerPawn();
	if (!IsValid(OwnerPawn)) return;

	UAmmoComponent* AmmoComponent = OwnerPawn->FindComponentByClass<UAmmoComponent>();
	if (!IsValid(AmmoComponent)) return;

	OnAmmoInitializedDelegate.BindUObject(this, &UAmmoWidget::SetAmmoText);
	OnAmmoChangedDelegate.BindUObject(this, &UAmmoWidget::SetAmmoText);
	OnAmmoReplenishRemainingTime.BindUObject(this, &UAmmoWidget::DisplayReplenishAmmoRemainingTime);

	OnAmmoInitializedDelegateHandle = AmmoComponent->AddOnAmmoInitializedHandler(OnAmmoInitializedDelegate);
	OnAmmoChangedDelegateHandle = AmmoComponent->AddOnAmmoChangedHandler(OnAmmoChangedDelegate);
	OnAmmoReplenishRemainingTimeHandle = AmmoComponent->AddAmmoReplenishRemainingTime(OnAmmoReplenishRemainingTime);
}

void UAmmoWidget::DisplayReplenishAmmoRemainingTime(const float RemainingTime) const
{
	if (!IsValid(AmmoText)) return;

	const FString RemainingTimeMessage = FString::Printf(TEXT("Replenishing ammo in: %.1f"), RemainingTime);
	AmmoText->SetText(FText::FromString(RemainingTimeMessage));
}