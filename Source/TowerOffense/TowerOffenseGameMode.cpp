#include "TowerOffenseGameMode.h"

#include "TankPawn.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogTowerOffenseGameMode)

void ATowerOffenseGameMode::BeginPlay()
{
	Super::BeginPlay();

	CheckSetup();
	SetupDelegates();
	SetupPostBeginPlayEnemiesCountUpdate();
}

void ATowerOffenseGameMode::SetupPostBeginPlayEnemiesCountUpdate()
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	World->GetTimerManager().SetTimerForNextTick(this, &ATowerOffenseGameMode::SetupEnemyCount);
}

void ATowerOffenseGameMode::SetEnemiesCount(int32 NewEnemiesCount)
{
	EnemyCount = NewEnemiesCount;
	EnemiesCountChanged.Broadcast(EnemyCount);
}

void ATowerOffenseGameMode::SetupEnemyCount()
{
	UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(World, EnemyClass, FoundActors);

	SetEnemiesCount(FoundActors.Num());
}

void ATowerOffenseGameMode::SetupDelegates()
{
	SetupOnEnemySpawnedDelegate();
	SetupOnEnemyDestroyedDelegate();
	SetupOnPlayerDestroyedDelegate();
}

void ATowerOffenseGameMode::SetupOnEnemySpawnedDelegate()
{
	UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	OnEnemySpawnedDelegate.BindUObject(this, &ATowerOffenseGameMode::OnEnemySpawned);
	OnEnemySpawnedDelegateHandle = World->AddOnActorSpawnedHandler(OnEnemySpawnedDelegate);
}

void ATowerOffenseGameMode::OnEnemySpawned(AActor* Actor)
{
	if (!IsValid(Actor) || !IsValid(EnemyClass) || !Actor->IsA(EnemyClass)) return;

	SetEnemiesCount(EnemyCount + 1);
}

void ATowerOffenseGameMode::SetupOnEnemyDestroyedDelegate()
{
	UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	OnEnemyDestroyedDelegate.BindUObject(this, &ATowerOffenseGameMode::OnEnemyDestroyed);
	OnEnemyDestroyedDelegateHandle = World->AddOnActorDestroyedHandler(OnEnemyDestroyedDelegate);
}

void ATowerOffenseGameMode::SetupOnPlayerDestroyedDelegate()
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	const APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!IsValid(PlayerController)) return;

	AActor* PlayerPawn = PlayerController->GetPawn();
	if (!IsValid(PlayerPawn)) return;

	PlayerPawn->OnDestroyed.AddDynamic(this, &ATowerOffenseGameMode::OnPlayerLoses);
}

void ATowerOffenseGameMode::OnEnemyDestroyed(AActor* Actor)
{
	if (!IsValid(Actor) || !IsValid(EnemyClass) || !Actor->IsA(EnemyClass)) return;

	SetEnemiesCount(EnemyCount - 1);
	CheckWinCondition();
}

void ATowerOffenseGameMode::OnPlayerLoses(AActor* Actor)
{
	PlayerLosesDelegate.Broadcast();
}

void ATowerOffenseGameMode::CheckSetup() const
{
	if (!IsValid(EnemyClass))
	{
		UE_LOG(LogTowerOffenseGameMode, Error, TEXT("EnemyClass in is not set in TowerOffenseGameMode"));
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("EnemyClass in is not set in TowerOffenseGameMode"));
		}
	}
}

void ATowerOffenseGameMode::CheckWinCondition() const
{
	if (EnemyCount > 0) return;

	PlayerWinsDelegate.Broadcast();
}

FDelegateHandle ATowerOffenseGameMode::AddPlayerWinsHandler(const FPlayerWinsDelegate::FDelegate& Delegate)
{
	return PlayerWinsDelegate.Add(Delegate);
}

FDelegateHandle ATowerOffenseGameMode::AddPlayerLosesHandler(const FPlayerWinsDelegate::FDelegate& Delegate)
{
	return PlayerLosesDelegate.Add(Delegate);
}

FDelegateHandle ATowerOffenseGameMode::AddEnemiesCountChangedHandler(const FOnEnemiesCountChanged::FDelegate& Delegate)
{
	return EnemiesCountChanged.Add(Delegate);
}