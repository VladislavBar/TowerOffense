#pragma once

#include "CoreMinimal.h"
#include "HealthWidget.h"
#include "Components/WidgetComponent.h"
#include "HealthBarWidgetComponent.generated.h"

UCLASS()
class TOWEROFFENSE_API UHealthBarWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Health")
	TSubclassOf<UHealthWidget> HealthWidgetClass;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	virtual void BeginPlay() override;
};
