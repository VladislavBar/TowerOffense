#pragma once

#include "CoreMinimal.h"
#include "HealthComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "HealthWidget.generated.h"

UCLASS()
class TOWEROFFENSE_API UHealthWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditInstanceOnly, meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthBar;

	UPROPERTY(EditInstanceOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> HealthText;

	UPROPERTY()
	AActor* Owner;

	bool SetupHealthComponentDelegate(UHealthComponent* HealthComponent);
	bool SetupHealthComponentAsWidgetComponentDelegate();
	bool SetupHealthComponentAsViewportDelegate();
	void TrySetupHealthComponentDelegate();
	void UpdateHealthProgressBar(const FHitTakenData& HitTakenData);
	virtual void NativeConstruct() override;
	void SetCurrentHealthBar(const float CurrentHealth, const float MaxHealth);
	void SetCurrentHealthText(const float CurrentHealth, const float MaxHealth);

public:
	void SetOwner(AActor* NewOwner) { Owner = NewOwner; }
};
