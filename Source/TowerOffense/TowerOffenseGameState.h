#pragma once

#include "CoreMinimal.h"
#include "TankPawn.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/GameStateBase.h"
#include "TowerOffenseGameState.generated.h"

DECLARE_MULTICAST_DELEGATE(FPlayerWinsDelegate);
DECLARE_MULTICAST_DELEGATE(FPlayerLosesDelegate);
DECLARE_MULTICAST_DELEGATE(FOnDelayStartDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnDelayRemainingTimeDelegate, float);
DECLARE_MULTICAST_DELEGATE(FOnMatchStartedDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMatchFinished, ETeam);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnParticipantsAmountChangedDelegate, const TArray<ATurretPawn*>&);

struct FOnTeamChangedDelegateWithHandle
{
	FOnTeamChangedDelegate::FDelegate Delegate;
	FDelegateHandle Handle;
	ATurretPawn* TurretPawn;
};

UENUM()
enum class ETowerOffenseMatchState
{
	WaitingToStart,
	InProgress,
	Finished
};

UCLASS()
class TOWEROFFENSE_API ATowerOffenseGameState : public AGameStateBase
{
	GENERATED_BODY()

	friend class ATowerOffenseGameMode;

public:
	ATowerOffenseGameState();

private:
	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	ETowerOffenseMatchState MatchState;

	UPROPERTY(ReplicatedUsing = OnRep_ActiveParticipants)
	TArray<ATurretPawn*> ActiveParticipants;

	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	float InitialTimeBeforeStart = 40.f;

	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	ETeam WinningTeam;

	FPlayerWinsDelegate PlayerWinsDelegate;
	FPlayerLosesDelegate PlayerLosesDelegate;

	FOnDelayStartDelegate StartMatchDelayStartedDelegate;
	FOnDelayRemainingTimeDelegate TickRemainingTimeBeforeMatchStartsDelegate;
	FOnMatchStartedDelegate OnMatchStartedDelegate;
	FOnMatchFinished OnMatchFinishedDelegate;

	FOnParticipantsAmountChangedDelegate ParticipantsAmountChanged;

	FTimerHandle TickDelayBeforeMatchStartsHandle;
	TArray<FOnTeamChangedDelegateWithHandle> OnTeamChangedDelegatesWithHandles;

private:
	UFUNCTION()
	void OnRep_ActiveParticipants();

	UFUNCTION()
	void OnRep_MatchState();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastMatchStartDelayStarted(const float DelayBeforeStart);
	void ActivateMatchStartDelayTimer();
	void ActivateMatchStartDelayTimerNextTick();

	void BroadcastRemainingTimeBeforeMatchStarts() const;
	void OnMatchWaitingToStart();
	void OnMatchStarted();
	void OnMatchFinished();
	void ScheduleOnMatchUpdatedOnNextTick();
	void OnMatchUpdated();

	void AddParticipant(ATurretPawn* NewParticipant);
	void RemoveParticipant(ATurretPawn* Participant);

	void OnTeamChanged(const ATurretPawn* TurretPawn, ETeam NewTeam);

	void ReSetupOnTeamChangeDelegates();
	void SetupOnTeamChangeDelegates();
	void DeleteTeamChangedDelegates();

	UFUNCTION(Client, Reliable)
	void SyncClientToMatchState();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaSeconds) override;
	// stub method meant to be use as callback for delay timer. We don't really care about finished event since
	// a server will notify us about that but we care about remaining time
	void DummyOnDelayFinished() const;

public:
	TArray<ATurretPawn*> GetActiveParticipants() const { return ActiveParticipants; }
	TArray<ATurretPawn*> GetActiveEnemies(ETeam Team) const;
	FDelegateHandle AddDelayStartHandler(const FOnDelayStartDelegate::FDelegate& Delegate);
	FDelegateHandle AddMatchStartedHandler(const FOnMatchStartedDelegate::FDelegate& Delegate);
	FDelegateHandle AddDelayRemainingTimeHandler(const FOnDelayRemainingTimeDelegate::FDelegate& Delegate);
	FDelegateHandle AddPlayersAmountChangedHandler(const FOnParticipantsAmountChangedDelegate::FDelegate& Delegate);
	FDelegateHandle AddMatchFinishedHandler(const FOnMatchFinished::FDelegate& Delegate);
	void RemovePlayersAmountChangedHandler(const FDelegateHandle& Handle);

	bool IsWaitingForStart() const;
	bool IsInProgress() const;

	UFUNCTION(Server, Reliable)
	void ServerSyncUserState(ATurretPawn* TurretPawn) const;
};
