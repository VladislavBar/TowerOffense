#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "EndScreenHUD.generated.h"

UCLASS()
class TOWEROFFENSE_API UEndScreenHUD : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditInstanceOnly, Category = "EndScreen", meta = (BindWidget))
	TObjectPtr<UButton> RestartButton;

	UPROPERTY(EditInstanceOnly, Category = "EndScreen", meta = (BindWidget))
	TObjectPtr<UButton> ExitButton;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void RestartGame();

	UFUNCTION()
	void ExitGame();
};
