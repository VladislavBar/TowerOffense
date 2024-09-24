#include "HealthBarWidgetComponent.h"

void UHealthBarWidgetComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (!IsValid(HealthWidgetClass)) return Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UHealthBarWidgetComponent, WidgetClass)
		|| PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UHealthBarWidgetComponent, HealthWidgetClass))
	{
		SetWidgetClass(HealthWidgetClass);
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UHealthBarWidgetComponent::BeginPlay()
{
	Super::BeginPlay();

	UHealthWidget* HealthWidget = Cast<UHealthWidget>(GetWidget());
	if (!IsValid(HealthWidget)) return;

	HealthWidget->SetOwner(GetOwner());
}