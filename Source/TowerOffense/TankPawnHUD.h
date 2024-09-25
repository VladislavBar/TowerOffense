#pragma once

#include "CoreMinimal.h"
#include "CooldownWidget.h"
#include "CrosshairWidget.h"
#include "EnemiesCountWidget.h"
#include "HealthWidget.h"
#include "Blueprint/UserWidget.h"
#include "TankPawnHUD.generated.h"

UCLASS()
class TOWEROFFENSE_API UTankPawnHUD : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditInstanceOnly, meta = (BindWidget))
	TObjectPtr<UCrosshairWidget> CrosshairWidget;

	UPROPERTY(EditInstanceOnly, meta = (BindWidget))
	TObjectPtr<UHealthWidget> HealthWidget;

	UPROPERTY(EditInstanceOnly, meta = (BindWidget))
	TObjectPtr<UCooldownWidget> CooldownWidget;

	UPROPERTY(EditInstanceOnly, meta = (BindWidget))
	TObjectPtr<UEnemiesCountWidget> EnemiesLeftWidget;
};
