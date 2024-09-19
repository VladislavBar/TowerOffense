#pragma once

#include "CoreMinimal.h"
#include "CooldownWidget.h"
#include "CrosshairWidget.h"
#include "GameFramework/PlayerController.h"
#include "TankPlayerController.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTankPawn, Log, All);

UCLASS()
class TOWEROFFENSE_API ATankPlayerController : public APlayerController
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<UCrosshairWidget> CrosshairWidgetClass;

	UPROPERTY()
	TObjectPtr<UCrosshairWidget> CrosshairWidget;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UCooldownWidget> CooldownWidgetClass;

	UPROPERTY()
	TObjectPtr<UCooldownWidget> CooldownWidget;

	virtual void BeginPlay() override;
	virtual void Tick(const float DeltaSeconds) override;
	void ResetCursor();
	void CrosshairFollowMouse();
	void SetupCrosshairWidget();
	void SetupCooldownWidget();

public:
	void RefreshCooldownWidget(float RemainingCooldownTime);
};
