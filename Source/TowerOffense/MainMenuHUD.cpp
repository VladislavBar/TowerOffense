#include "MainMenuHUD.h"

#include "Kismet/GameplayStatics.h"

void UMainMenuHUD::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(StartButton))
	{
		StartButton->OnClicked.AddDynamic(this, &UMainMenuHUD::StartGame);
	}

	if (IsValid(QuitButton))
	{
		QuitButton->OnClicked.AddDynamic(this, &UMainMenuHUD::QuitGame);
	}
}

void UMainMenuHUD::StartGame()
{
	if (LevelToLoad.IsNull()) return;
	EnsureLevelIsLoaded();

	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	UGameplayStatics::OpenLevelBySoftObjectPtr(World, LevelToLoad);
}

void UMainMenuHUD::QuitGame()
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!IsValid(PlayerController)) return;

	UKismetSystemLibrary::QuitGame(World, PlayerController, EQuitPreference::Quit, true);
}

void UMainMenuHUD::EnsureLevelIsLoaded()
{
	if (!LevelToLoad.IsNull() && LevelToLoad.IsPending())
	{
		LevelToLoad.LoadSynchronous();
	}
}