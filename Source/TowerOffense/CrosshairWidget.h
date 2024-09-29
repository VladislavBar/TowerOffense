#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CrosshairWidget.generated.h"

class UImage;
UCLASS(Abstract)
class TOWEROFFENSE_API UCrosshairWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditInstanceOnly, meta = (BindWidget))
	TObjectPtr<UImage> CrosshairImage;
};
