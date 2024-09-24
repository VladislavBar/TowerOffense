#include "EndScreenHUD.h"

#include "Kismet/GameplayStatics.h"

void UEndScreenHUD::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(RestartButton))
	{
		RestartButton->OnClicked.AddDynamic(this, &UEndScreenHUD::RestartGame);
	}

	if (IsValid(ExitButton))
	{
		ExitButton->OnClicked.AddDynamic(this, &UEndScreenHUD::ExitGame);
	}
}

void UEndScreenHUD::RestartGame()
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	const ULevel* CurrentLevel = GetWorld()->GetCurrentLevel();
	if (!IsValid(CurrentLevel)) return;

	UGameplayStatics::OpenLevel(World, CurrentLevel->GetFName());
}

void UEndScreenHUD::ExitGame()
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!IsValid(PlayerController)) return;

	UKismetSystemLibrary::QuitGame(World, PlayerController, EQuitPreference::Quit, true);
}
