#pragma once

#include "CoreMinimal.h"
#include "CrosshairWidget.h"
#include "GameFramework/PlayerController.h"
#include "TankPlayerController.generated.h"

UCLASS()
class TOWEROFFENSE_API ATankPlayerController : public APlayerController
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<UCrosshairWidget> CrosshairWidgetClass;

	UPROPERTY()
	TObjectPtr<UCrosshairWidget> CrosshairWidget;

	virtual void BeginPlay() override;
	virtual void Tick(const float DeltaSeconds) override;
	void ResetCursor();
	void CrosshairFollowMouse();
};
