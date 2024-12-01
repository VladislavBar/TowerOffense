#include "TowerOffenseGameMode.h"

#include "TankPawn.h"
#include "TeamHelper.h"
#include "TowerOffenseGameState.h"
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

	PrepareGame();
	SetupDelegates();
	ScheduleStartDelayOnNextTick();
	SetupAmbientSound();
}

void ATowerOffenseGameMode::PrepareGame() const
{
	SetMatchState(ETowerOffenseMatchState::WaitingToStart);
	MulticastGameStateDelayStarted(DelayTime);
}

void ATowerOffenseGameMode::SetMatchState(ETowerOffenseMatchState MatchState) const
{
	ATowerOffenseGameState* CurrentGameState = GetGameState<ATowerOffenseGameState>();
	if (!IsValid(CurrentGameState)) return;

	CurrentGameState->MatchState = MatchState;
	CurrentGameState->OnRep_MatchState();
}

void ATowerOffenseGameMode::SetupParticipants()
{
	ATowerOffenseGameState* CurrentGameState = GetGameState<ATowerOffenseGameState>();
	if (!IsValid(CurrentGameState)) return;

	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	TArray<AActor*> Participants;
	UGameplayStatics::GetAllActorsOfClass(World, ATurretPawn::StaticClass(), Participants);

	TArray<ATurretPawn*> TurretParticipants;
	for (AActor* Actor : Participants)
	{
		ATurretPawn* TurretPawn = Cast<ATurretPawn>(Actor);
		if (!IsValid(TurretPawn)) continue;

		TurretParticipants.Add(TurretPawn);
	}

	CurrentGameState->ActiveParticipants = TurretParticipants;
	CurrentGameState->OnRep_ActiveParticipants();
}

void ATowerOffenseGameMode::ScheduleStartMatch()
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	World->GetTimerManager().SetTimer(DelayTimerHandle, this, &ATowerOffenseGameMode::StartMatch, DelayTime);
	MulticastGameStateDelayStarted(DelayTime);
}

void ATowerOffenseGameMode::SetupDelegates()
{
	SetupOnParticipantSpawnedDelegate();
	SetupOnParticipantDestroyedDelegate();
}

void ATowerOffenseGameMode::SetupOnParticipantSpawnedDelegate()
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	OnActorSpawnedDelegate.BindUObject(this, &ATowerOffenseGameMode::OnPawnSpawned);
	OnEnemySpawnedDelegateHandle = World->AddOnActorSpawnedHandler(OnActorSpawnedDelegate);
}

void ATowerOffenseGameMode::SetupOnParticipantDestroyedDelegate()
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	OnActorDestroyedDelegate.BindUObject(this, &ATowerOffenseGameMode::OnParticipantDestroyed);
	OnEnemyDestroyedDelegateHandle = World->AddOnActorDestroyedHandler(OnActorDestroyedDelegate);
}

void ATowerOffenseGameMode::ScheduleStartDelayOnNextTick()
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	World->GetTimerManager().SetTimerForNextTick(this, &ATowerOffenseGameMode::OnStartDelay);
}

void ATowerOffenseGameMode::SetupAmbientSound() const
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	UGameplayStatics::SpawnSoundAtLocation(World, AmbientSound, FVector::ZeroVector);
}

void ATowerOffenseGameMode::MulticastGameStateDelayStarted(const float DelayBeforeStart) const
{
	ATowerOffenseGameState* CurrentGameState = GetGameState<ATowerOffenseGameState>();
	if (!IsValid(CurrentGameState)) return;

	CurrentGameState->MulticastMatchStartDelayStarted(DelayBeforeStart);
}

bool ATowerOffenseGameMode::HasAnyTeamWon() const
{
	const ATowerOffenseGameState* CurrentGameState = GetGameState<ATowerOffenseGameState>();
	if (!IsValid(CurrentGameState)) return false;

	return FTeamHelper::GetTeams(CurrentGameState->ActiveParticipants).Num() == 1;
}

void ATowerOffenseGameMode::SyncRemainingTimeBeforeMatchStarts() const
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	ATowerOffenseGameState* CurrentGameState = GetGameState<ATowerOffenseGameState>();
	if (!IsValid(CurrentGameState) || !CurrentGameState->IsWaitingForStart()) return;

	const float RemainingTime = World->GetTimerManager().GetTimerRemaining(DelayTimerHandle);
	CurrentGameState->InitialTimeBeforeStart = RemainingTime;
}

void ATowerOffenseGameMode::OnPawnSpawned(AActor* Actor)
{
	OnParticipantSpawned(Actor);
}

void ATowerOffenseGameMode::OnParticipantSpawned(AActor* Actor)
{
	ATurretPawn* TurretPawn = Cast<ATurretPawn>(Actor);
	if (!IsValid(TurretPawn)) return;

	ATowerOffenseGameState* CurrentGameState = GetGameState<ATowerOffenseGameState>();
	if (!IsValid(CurrentGameState)) return;

	TurretPawn->OnDestroyed.AddDynamic(this, &ATowerOffenseGameMode::OnParticipantDestroyed);
	TurretPawn->MulticastSetActorTickEnabled(false);

	CurrentGameState->AddParticipant(TurretPawn);
}

void ATowerOffenseGameMode::OnParticipantDestroyed(AActor* Actor)
{
	ATurretPawn* TurretPawn = Cast<ATurretPawn>(Actor);
	if (!IsValid(TurretPawn)) return;

	ATowerOffenseGameState* CurrentGameState = GetGameState<ATowerOffenseGameState>();
	if (!IsValid(CurrentGameState)) return;

	CurrentGameState->RemoveParticipant(TurretPawn);
	TryFinishingMatch();
}

void ATowerOffenseGameMode::OnStartDelay()
{
	SetupParticipants();
	DisableParticipantsTick();
	ScheduleStartMatch();
}

void ATowerOffenseGameMode::ToggleParticipantsTick(bool bShouldTick) const
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	const ATowerOffenseGameState* CurrentGameState = GetGameState<ATowerOffenseGameState>();
	if (!IsValid(CurrentGameState)) return;

	for (ATurretPawn* Actor : CurrentGameState->GetActiveParticipants())
	{
		if (!IsValid(Actor)) continue;

		Actor->MulticastSetActorTickEnabled(bShouldTick);
	}
}

void ATowerOffenseGameMode::DisableParticipantsTick() const
{
	ToggleParticipantsTick(false);
}

void ATowerOffenseGameMode::EnableParticipantsTick() const
{
	ToggleParticipantsTick(true);
}

void ATowerOffenseGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	SyncRemainingTimeBeforeMatchStarts();
}

void ATowerOffenseGameMode::ActivateParticipants() const
{
	const ATowerOffenseGameState* CurrentGameState = GetGameState<ATowerOffenseGameState>();
	if (!IsValid(CurrentGameState)) return;

	for (ATurretPawn* Participant : CurrentGameState->GetActiveParticipants())
	{
		if (!IsValid(Participant)) continue;

		Participant->MulticastSetActorTickEnabled(true);
	}
}

void ATowerOffenseGameMode::StartMatch() const
{
	ActivateParticipants();
	SetMatchState(ETowerOffenseMatchState::InProgress);
}

void ATowerOffenseGameMode::TryFinishingMatch()
{
	const TArray<ETeam> Teams = FTeamHelper::GetTeams(GetGameState<ATowerOffenseGameState>()->GetActiveParticipants());
	if (Teams.Num() > 1) return;

	const ETeam WinningTeam = Teams.Num() == 1 ? Teams[0] : ETeam::None;
	ScheduleEndMatchDelay(WinningTeam);
}

void ATowerOffenseGameMode::ScheduleEndMatchDelay(const ETeam WinningTeam)
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	OnFinishDelayDelegate.BindUObject(this, &ATowerOffenseGameMode::FinishMatch, WinningTeam);
	World->GetTimerManager().SetTimer(OnFinishDelayHandle, OnFinishDelayDelegate, DelayTime, false);
}

void ATowerOffenseGameMode::FinishMatch(const ETeam WinningTeam) const
{
	ATowerOffenseGameState* CurrentGameState = GetGameState<ATowerOffenseGameState>();
	if (!IsValid(CurrentGameState)) return;

	CurrentGameState->WinningTeam = WinningTeam;
	SetMatchState(ETowerOffenseMatchState::Finished);
}

void ATowerOffenseGameMode::SyncUserState(ATurretPawn* TurretPawn) const
{
	ATowerOffenseGameState* CurrentGameState = GetGameState<ATowerOffenseGameState>();
	if (!IsValid(CurrentGameState)) return;

	TurretPawn->OnRep_CurrentTeam();
	const bool bShouldUserBeActive = CurrentGameState->IsInProgress();

	TurretPawn->MulticastSetActorTickEnabled(bShouldUserBeActive);
	CurrentGameState->SyncClientToMatchState();
}
