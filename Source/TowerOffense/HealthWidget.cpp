#include "HealthWidget.h"

#include "HealthComponent.h"
#include "TurretPawn.h"
#include "Components/PanelWidget.h"
#include "Components/WidgetComponent.h"

bool UHealthWidget::SetupHealthComponentDelegate(UHealthComponent* HealthComponent)
{
	if (!IsValid(HealthComponent)) return false;

	OnHitTakenDelegate.BindUObject(this, &UHealthWidget::UpdateHealthProgressBar);
	OnHitTakenDelegateHandle = HealthComponent->AddOnHitTakenHandler(OnHitTakenDelegate);
	SetCurrentHealthBar(HealthComponent->GetHealthPoints(), HealthComponent->GetMaxHealth());
	SetCurrentHealthText(HealthComponent->GetHealthPoints(), HealthComponent->GetMaxHealth());

	return true;
}

bool UHealthWidget::SetupHealthComponentAsWidgetComponentDelegate()
{
	if (!IsValid(Owner)) return false;

	return SetupHealthComponentDelegate(Owner->GetComponentByClass<UHealthComponent>());
}

bool UHealthWidget::SetupHealthComponentAsViewportDelegate()
{
	if (!IsValid(GetParent()) || GetParent()->IsA<UWidgetComponent>()) return false;

	const AActor* TurretPawn = GetOwningPlayerPawn();
	if (!IsValid(TurretPawn)) return false;

	UHealthComponent* HealthComponent = TurretPawn->FindComponentByClass<UHealthComponent>();
	return SetupHealthComponentDelegate(HealthComponent);
}

void UHealthWidget::TrySetupHealthComponentDelegate()
{
	if (SetupHealthComponentAsViewportDelegate()) return;
	SetupHealthComponentAsWidgetComponentDelegate();
}

void UHealthWidget::UpdateHealthProgressBar(const FHitTakenData& HitTakenData)
{
	if (!IsValid(HealthBar) || HitTakenData.MaxHealth <= UE_KINDA_SMALL_NUMBER) return;

	SetCurrentHealthBar(HitTakenData.CurrentHealth, HitTakenData.MaxHealth);
	SetCurrentHealthText(HitTakenData.CurrentHealth, HitTakenData.MaxHealth);
}

void UHealthWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TrySetupHealthComponentDelegate();
}

void UHealthWidget::SetCurrentHealthBar(const float CurrentHealth, const float MaxHealth)
{
	if (!IsValid(HealthBar) || MaxHealth <= UE_KINDA_SMALL_NUMBER) return;

	HealthBar->SetPercent(CurrentHealth / MaxHealth);
}

void UHealthWidget::SetCurrentHealthText(const float CurrentHealth, const float MaxHealth)
{
	if (!IsValid(HealthText)) return;

	HealthText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), CurrentHealth, MaxHealth)));
}