#include "EndScreenHUD.h"

#include "Kismet/GameplayStatics.h"

void UEndScreenHUD::RestartGame() const
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	const ULevel* CurrentLevel = GetWorld()->GetCurrentLevel();
	if (!IsValid(CurrentLevel)) return;

	UGameplayStatics::OpenLevel(World, CurrentLevel->GetFName());
}

void UEndScreenHUD::ExitGame() const
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!IsValid(PlayerController)) return;

	UKismetSystemLibrary::QuitGame(World, PlayerController, EQuitPreference::Quit, true);
}
