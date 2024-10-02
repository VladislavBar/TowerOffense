#include "TowerOffenseGameMode.h"

#include "TankPawn.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogTowerOffenseGameMode)

ATowerOffenseGameMode::ATowerOffenseGameMode()
{
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
}

void ATowerOffenseGameMode::BeginPlay()
{
	Super::BeginPlay();

	CheckSetup();
	SetupDelegates();
	SetupPostBeginPlayEnemiesCountUpdate();
	SetupStartDelay();
	SetupFinishDelay();
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

	PlayerPawn->OnDestroyed.AddDynamic(this, &ATowerOffenseGameMode::SetupOnPlayerLosesEndMatchTimer);
}

void ATowerOffenseGameMode::SetupStartDelay()
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	DisableSelectedActorsTick();
	World->GetTimerManager().SetTimerForNextTick(this, &ATowerOffenseGameMode::OnStartDelay);
}

void ATowerOffenseGameMode::SetupFinishDelay()
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	World->GetTimerManager().SetTimer(DelayTimerHandle, this, &ATowerOffenseGameMode::OnFinishDelay, DelayTime, false);
}

void ATowerOffenseGameMode::SetupEndMatchDelay(FTimerDelegate::TMethodPtr<ATowerOffenseGameMode> InTimerMethod)
{
	UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	World->GetTimerManager().SetTimer(MatchEndedTimerHandle, this, InTimerMethod, MatchEndDelayTime, false);
}

void ATowerOffenseGameMode::SetupOnPlayerLosesEndMatchTimer(AActor* Actor)
{
	SetupEndMatchDelay(&ATowerOffenseGameMode::OnPlayerLoses);
}

void ATowerOffenseGameMode::OnEnemyDestroyed(AActor* Actor)
{
	if (!IsValid(Actor) || !IsValid(EnemyClass) || !Actor->IsA(EnemyClass)) return;

	SetEnemiesCount(EnemyCount - 1);
	CheckAndSetupWinCondition();
}

void ATowerOffenseGameMode::OnStartDelay()
{
	DisableSelectedActorsTick();
	DelayStartDelegate.Broadcast();
}

void ATowerOffenseGameMode::OnFinishDelay()
{
	EnableSelectedActorsTick();
	DelayFinishDelegate.Broadcast();
}

void ATowerOffenseGameMode::OnPlayerWins()
{
	PlayerWinsDelegate.Broadcast();
}

void ATowerOffenseGameMode::OnPlayerLoses()
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

void ATowerOffenseGameMode::CheckAndSetupWinCondition()
{
	if (EnemyCount > 0) return;

	SetupEndMatchDelay(&ATowerOffenseGameMode::OnPlayerWins);
}

void ATowerOffenseGameMode::ToggleSelectedActorsTick(bool bShouldTick) const
{
	if (!IsValid(DelayStartActorClass)) return;

	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(World, DelayStartActorClass, FoundActors);

	for (AActor* Actor : FoundActors)
	{
		if (!IsValid(Actor)) continue;

		Actor->SetActorTickEnabled(bShouldTick);
	}
}

void ATowerOffenseGameMode::DisableSelectedActorsTick()
{
	bHasStarted = false;
	ToggleSelectedActorsTick(false);
}

void ATowerOffenseGameMode::EnableSelectedActorsTick()
{
	bHasStarted = true;
	ToggleSelectedActorsTick(true);
}

void ATowerOffenseGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	BroadcastRemainingTime();
}

void ATowerOffenseGameMode::BroadcastRemainingTime() const
{
	if (bHasStarted) return;

	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	const float TimerRemaining = World->GetTimerManager().GetTimerRemaining(DelayTimerHandle);
	DelayRemainingTimeDelegate.Broadcast(TimerRemaining);
}

FDelegateHandle ATowerOffenseGameMode::AddPlayerWinsHandler(const FPlayerWinsDelegate::FDelegate& Delegate)
{
	return PlayerWinsDelegate.Add(Delegate);
}

FDelegateHandle ATowerOffenseGameMode::AddPlayerLosesHandler(const FPlayerWinsDelegate::FDelegate& Delegate)
{
	return PlayerLosesDelegate.Add(Delegate);
}

FDelegateHandle ATowerOffenseGameMode::AddEnemiesCountChangedHandler(const FOnEnemiesCountChangedDelegate::FDelegate& Delegate)
{
	return EnemiesCountChanged.Add(Delegate);
}

FDelegateHandle ATowerOffenseGameMode::AddDelayStartHandler(const FOnDelayStartDelegate::FDelegate& Delegate)
{
	return DelayStartDelegate.Add(Delegate);
}

FDelegateHandle ATowerOffenseGameMode::AddDelayFinishHandler(const FOnDelayStartDelegate::FDelegate& Delegate)
{
	return DelayFinishDelegate.Add(Delegate);
}

FDelegateHandle ATowerOffenseGameMode::AddDelayRemainingTimeHandler(const FOnDelayRemainingTimeDelegate::FDelegate& Delegate)
{
	return DelayRemainingTimeDelegate.Add(Delegate);
}
