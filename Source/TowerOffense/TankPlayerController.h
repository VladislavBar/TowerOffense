#pragma once

#include "CoreMinimal.h"
#include "TankPawnHUD.h"
#include "GameFramework/PlayerController.h"
#include "TankPlayerController.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTankPawn, Log, All);

UCLASS()
class TOWEROFFENSE_API ATankPlayerController : public APlayerController
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UTankPawnHUD> TankPawnHUDClass;

	UPROPERTY()
	TObjectPtr<UTankPawnHUD> TankPawnHUD;

	virtual void BeginPlay() override;
	virtual void Tick(const float DeltaSeconds) override;
	void ResetCursor();
	void SetupTankHUD();
};
