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

	const FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(World, true);
	if (CurrentLevelName.IsEmpty()) return;

	UGameplayStatics::OpenLevel(World, *CurrentLevelName);
}

void UEndScreenHUD::ExitGame()
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!IsValid(PlayerController)) return;

	UKismetSystemLibrary::QuitGame(World, PlayerController, EQuitPreference::Quit, true);
}
