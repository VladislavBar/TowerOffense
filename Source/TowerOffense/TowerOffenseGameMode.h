#pragma once

#include "CoreMinimal.h"
#include "TowerOffenseGameState.h"
#include "TurretPawn.h"
#include "GameFramework/GameStateBase.h"
#include "TowerOffenseGameMode.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTowerOffenseGameMode, Log, All);

UCLASS()
class TOWEROFFENSE_API ATowerOffenseGameMode : public AGameModeBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Start", meta = (ClampMin = "0.0"))
	float DelayTime = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = "End", meta = (ClampMin = "0.0"))
	float MatchEndDelayTime = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "SFX")
	TObjectPtr<USoundBase> AmbientSound;

	FOnActorSpawned::FDelegate OnActorSpawnedDelegate;
	FOnActorDestroyed::FDelegate OnActorDestroyedDelegate;
	FOnActorDestroyed::FDelegate OnPlayerDestroyedDelegate;

	FDelegateHandle OnEnemySpawnedDelegateHandle;
	FDelegateHandle OnEnemyDestroyedDelegateHandle;

	FTimerHandle DelayTimerHandle;
	FTimerHandle MatchEndedTimerHandle;

	FTimerDelegate OnFinishDelayDelegate;
	FTimerHandle OnFinishDelayHandle;

	TArray<FOnTeamChangedDelegateWithHandle> OnTeamChangedDelegatesWithHandles;

public:
	ATowerOffenseGameMode();

private:
	virtual void BeginPlay() override;
	void PrepareGame() const;

	void SetMatchState(ETowerOffenseMatchState MatchState) const;
	void SetupParticipants();
	void ScheduleStartMatch();

	void SetupDelegates();
	void SetupOnParticipantSpawnedDelegate();
	void SetupOnParticipantDestroyedDelegate();
	void SetupAmbientSound() const;
	void MulticastGameStateDelayStarted(const float DelayBeforeStart) const;
	bool HasAnyTeamWon() const;

	UFUNCTION()
	void SyncRemainingTimeBeforeMatchStarts() const;

	void OnPawnSpawned(AActor* Actor);
	void OnParticipantSpawned(AActor* Actor);

	UFUNCTION()
	void OnParticipantDestroyed(AActor* Actor);
	void OnStartDelay();

	void ToggleParticipantsTick(bool bShouldTick) const;
	void DisableParticipantsTick() const;
	void EnableParticipantsTick() const;

	virtual void Tick(float DeltaSeconds) override;
	void ActivateParticipants() const;

	void StartMatch() const;
	void TryFinishingMatch();
	void ScheduleEndMatchDelay(const ETeam WinningTeam);
	void FinishMatch(const ETeam WinningTeam) const;

public:
	void SyncUserState(ATurretPawn* TurretPawn) const;
	void ScheduleStartDelayOnNextTick();
};
