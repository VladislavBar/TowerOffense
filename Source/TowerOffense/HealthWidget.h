#pragma once

#include "CoreMinimal.h"
#include "HealthComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "HealthWidget.generated.h"

UCLASS(Abstract)
class TOWEROFFENSE_API UHealthWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditInstanceOnly, meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthBar;

	UPROPERTY(EditInstanceOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> HealthText;

	UPROPERTY()
	AActor* Owner;

	FOnHitTakenDelegate::FDelegate OnHitTakenDelegate;
	FDelegateHandle OnHitTakenDelegateHandle;

	bool SetupHealthComponentDelegate(UHealthComponent* HealthComponent);
	bool SetupHealthComponentAsWidgetComponentDelegate();
	bool SetupHealthComponentAsViewportDelegate();
	void TrySetupHealthComponentDelegate();
	void UpdateHealthProgressBar(const FHitTakenData& HitTakenData);

	void SetCurrentHealthBar(const float CurrentHealth, const float MaxHealth);
	void SetCurrentHealthText(const float CurrentHealth, const float MaxHealth);

	virtual void NativeConstruct() override;

public:
	void SetOwner(AActor* NewOwner) { Owner = NewOwner; }
};
