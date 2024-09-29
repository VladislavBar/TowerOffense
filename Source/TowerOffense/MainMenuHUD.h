#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "MainMenuHUD.generated.h"

UCLASS(Abstract)
class TOWEROFFENSE_API UMainMenuHUD : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> LevelToLoad;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> StartButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuitButton;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void StartGame();

	UFUNCTION()
	void QuitGame();

	void EnsureLevelIsLoaded();
};
