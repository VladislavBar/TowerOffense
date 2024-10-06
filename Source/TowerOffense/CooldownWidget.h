#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "CooldownWidget.generated.h"

UCLASS(Abstract)
class TOWEROFFENSE_API UCooldownWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> CooldownText;

public:
	void SetCooldownText(const float RemainingCooldownTime);
	virtual void NativeConstruct() override;

	void HideCooldownText() const;
	void ShowCooldownText() const;

private:
	void SetupOnCooldownTickDelegate();
};
