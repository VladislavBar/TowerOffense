#include "TowerOffenseGameState.h"

#include "TeamHelper.h"
#include "TowerOffenseGameMode.h"
#include "Net/UnrealNetwork.h"

ATowerOffenseGameState::ATowerOffenseGameState()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ATowerOffenseGameState::OnRep_MatchState()
{
	ScheduleOnMatchUpdatedOnNextTick();
}

void ATowerOffenseGameState::OnRep_ActiveParticipants()
{
	ReSetupOnTeamChangeDelegates();
	ParticipantsAmountChanged.Broadcast(ActiveParticipants);
}

void ATowerOffenseGameState::ActivateMatchStartDelayTimer()
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	if (World->GetTimerManager().IsTimerActive(TickDelayBeforeMatchStartsHandle)) return;

	World->GetTimerManager().SetTimer(TickDelayBeforeMatchStartsHandle, this, &ATowerOffenseGameState::DummyOnDelayFinished, InitialTimeBeforeStart);
	StartMatchDelayStartedDelegate.Broadcast();
}

void ATowerOffenseGameState::ActivateMatchStartDelayTimerNextTick()
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	World->GetTimerManager().SetTimerForNextTick(this, &ATowerOffenseGameState::ActivateMatchStartDelayTimer);
}

void ATowerOffenseGameState::MulticastMatchStartDelayStarted_Implementation(const float DelayBeforeStart)
{
	InitialTimeBeforeStart = DelayBeforeStart;
	ActivateMatchStartDelayTimer();
}

void ATowerOffenseGameState::BroadcastRemainingTimeBeforeMatchStarts() const
{
	if (MatchState != ETowerOffenseMatchState::WaitingToStart) return;

	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	const float TimerRemaining = World->GetTimerManager().GetTimerRemaining(TickDelayBeforeMatchStartsHandle);
	TickRemainingTimeBeforeMatchStartsDelegate.Broadcast(TimerRemaining);
}

void ATowerOffenseGameState::OnMatchWaitingToStart()
{
	ActivateMatchStartDelayTimerNextTick();
}

void ATowerOffenseGameState::OnMatchStarted()
{
	OnMatchStartedDelegate.Broadcast();
}

void ATowerOffenseGameState::OnMatchFinished()
{
	OnMatchFinishedDelegate.Broadcast(WinningTeam);
}

void ATowerOffenseGameState::ScheduleOnMatchUpdatedOnNextTick()
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	World->GetTimerManager().SetTimerForNextTick(this, &ATowerOffenseGameState::OnMatchUpdated);
}

void ATowerOffenseGameState::OnMatchUpdated()
{
	switch (MatchState)
	{
		case ETowerOffenseMatchState::WaitingToStart:
		{
			return OnMatchWaitingToStart();
		}
		case ETowerOffenseMatchState::InProgress:
		{
			return OnMatchStarted();
		}
		case ETowerOffenseMatchState::Finished:
		{
			return OnMatchFinished();
		}
	}
}

void ATowerOffenseGameState::AddParticipant(ATurretPawn* NewParticipant)
{
	ActiveParticipants.Add(NewParticipant);
	OnRep_ActiveParticipants();
}

void ATowerOffenseGameState::RemoveParticipant(ATurretPawn* Participant)
{
	ActiveParticipants.Remove(Participant);
	OnRep_ActiveParticipants();
}

void ATowerOffenseGameState::OnTeamChanged(const ATurretPawn* TurretPawn, ETeam NewTeam)
{
	ParticipantsAmountChanged.Broadcast(ActiveParticipants);
}

void ATowerOffenseGameState::ReSetupOnTeamChangeDelegates()
{
	DeleteTeamChangedDelegates();
	SetupOnTeamChangeDelegates();
}

void ATowerOffenseGameState::SetupOnTeamChangeDelegates()
{
	for (ATurretPawn* TurretPawn : ActiveParticipants)
	{
		if (!IsValid(TurretPawn)) return;

		FOnTeamChangedDelegateWithHandle OnTeamChangedDelegateWithHandle;
		OnTeamChangedDelegateWithHandle.Delegate.BindUObject(this, &ATowerOffenseGameState::OnTeamChanged);
		OnTeamChangedDelegateWithHandle.Handle = TurretPawn->AddOnTeamChangedHandler(OnTeamChangedDelegateWithHandle.Delegate);
		OnTeamChangedDelegateWithHandle.TurretPawn = TurretPawn;

		OnTeamChangedDelegatesWithHandles.Add(OnTeamChangedDelegateWithHandle);
	}
}

void ATowerOffenseGameState::DeleteTeamChangedDelegates()
{
	for (const FOnTeamChangedDelegateWithHandle& OnTeamChangedDelegateWithHandle : OnTeamChangedDelegatesWithHandles)
	{
		if (!IsValid(OnTeamChangedDelegateWithHandle.TurretPawn)) continue;

		OnTeamChangedDelegateWithHandle.TurretPawn->RemoveOnTeamChangedHandler(OnTeamChangedDelegateWithHandle.Handle);
	}

	OnTeamChangedDelegatesWithHandles.Empty();
}

void ATowerOffenseGameState::ServerSyncUserState_Implementation(ATurretPawn* TurretPawn) const
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	const ATowerOffenseGameMode* GameMode = World->GetAuthGameMode<ATowerOffenseGameMode>();
	if (!IsValid(GameMode)) return;

	GameMode->SyncUserState(TurretPawn);
}

void ATowerOffenseGameState::SyncClientToMatchState_Implementation()
{
	OnRep_MatchState();
	ParticipantsAmountChanged.Broadcast(GetActiveParticipants());
}

void ATowerOffenseGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATowerOffenseGameState, ActiveParticipants);
	DOREPLIFETIME(ATowerOffenseGameState, MatchState);
	DOREPLIFETIME_CONDITION(ATowerOffenseGameState, InitialTimeBeforeStart, COND_InitialOnly);
	DOREPLIFETIME(ATowerOffenseGameState, WinningTeam);
}

void ATowerOffenseGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	BroadcastRemainingTimeBeforeMatchStarts();
}

void ATowerOffenseGameState::DummyOnDelayFinished() const {}

TArray<ATurretPawn*> ATowerOffenseGameState::GetActiveEnemies(ETeam Team) const
{
	return FTeamHelper::GetEnemies(ActiveParticipants, Team);
}

FDelegateHandle ATowerOffenseGameState::AddDelayStartHandler(const FOnDelayStartDelegate::FDelegate& Delegate)
{
	return StartMatchDelayStartedDelegate.Add(Delegate);
}

FDelegateHandle ATowerOffenseGameState::AddMatchStartedHandler(const FOnDelayStartDelegate::FDelegate& Delegate)
{
	return OnMatchStartedDelegate.Add(Delegate);
}

FDelegateHandle ATowerOffenseGameState::AddDelayRemainingTimeHandler(const FOnDelayRemainingTimeDelegate::FDelegate& Delegate)
{
	return TickRemainingTimeBeforeMatchStartsDelegate.Add(Delegate);
}

FDelegateHandle ATowerOffenseGameState::AddPlayersAmountChangedHandler(const FOnParticipantsAmountChangedDelegate::FDelegate& Delegate)
{
	return ParticipantsAmountChanged.Add(Delegate);
}

FDelegateHandle ATowerOffenseGameState::AddMatchFinishedHandler(const FOnMatchFinished::FDelegate& Delegate)
{
	return OnMatchFinishedDelegate.Add(Delegate);
}

void ATowerOffenseGameState::RemovePlayersAmountChangedHandler(const FDelegateHandle& Handle)
{
	ParticipantsAmountChanged.Remove(Handle);
}

bool ATowerOffenseGameState::IsWaitingForStart() const
{
	return MatchState == ETowerOffenseMatchState::WaitingToStart;
}

bool ATowerOffenseGameState::IsInProgress() const
{
	return MatchState == ETowerOffenseMatchState::InProgress;
}