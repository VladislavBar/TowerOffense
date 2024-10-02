#pragma once

#include "CoreMinimal.h"
#include "TurretPawn.h"
#include "GameFramework/GameModeBase.h"
#include "TowerOffenseGameMode.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTowerOffenseGameMode, Log, All);

DECLARE_MULTICAST_DELEGATE(FPlayerWinsDelegate);
DECLARE_MULTICAST_DELEGATE(FPlayerLosesDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEnemiesCountChangedDelegate, int32);
DECLARE_MULTICAST_DELEGATE(FOnDelayStartDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnDelayRemainingTimeDelegate, float);
DECLARE_MULTICAST_DELEGATE(FOnDelayFinishDelegate);

UCLASS()
class TOWEROFFENSE_API ATowerOffenseGameMode : public AGameModeBase
{
	GENERATED_BODY()

	int32 EnemyCount = 0;
	bool bHasStarted = false;

	UPROPERTY(EditDefaultsOnly, Category = "Start", meta = (ClampMin = "0.0"))
	float DelayTime = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = "End", meta = (ClampMin = "0.0"))
	float MatchEndDelayTime = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Start")
	TSubclassOf<AActor> DelayStartActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy")
	TSubclassOf<ATurretPawn> EnemyClass;

	UPROPERTY(EditDefaultsOnly, Category = "SFX")
	TObjectPtr<USoundBase> AmbientSound;

	FOnActorSpawned::FDelegate OnEnemySpawnedDelegate;
	FOnActorDestroyed::FDelegate OnEnemyDestroyedDelegate;
	FOnActorDestroyed::FDelegate OnPlayerDestroyedDelegate;

	FPlayerWinsDelegate PlayerWinsDelegate;
	FPlayerLosesDelegate PlayerLosesDelegate;
	FOnEnemiesCountChangedDelegate EnemiesCountChanged;
	FOnDelayStartDelegate DelayStartDelegate;
	FOnDelayRemainingTimeDelegate DelayRemainingTimeDelegate;
	FOnDelayFinishDelegate DelayFinishDelegate;

	FDelegateHandle OnEnemySpawnedDelegateHandle;
	FDelegateHandle OnEnemyDestroyedDelegateHandle;

	FTimerHandle DelayTimerHandle;
	FTimerHandle MatchEndedTimerHandle;

public:
	ATowerOffenseGameMode();

private:
	virtual void BeginPlay() override;
	void SetupPostBeginPlayEnemiesCountUpdate();

	void SetEnemiesCount(int32 NewEnemiesCount);

	void SetupEnemyCount();
	void SetupDelegates();
	void SetupOnEnemySpawnedDelegate();
	void SetupOnEnemyDestroyedDelegate();
	void SetupOnPlayerDestroyedDelegate();
	void SetupStartDelay();
	void SetupFinishDelay();
	void SetupEndMatchDelay(FTimerDelegate::TMethodPtr<ATowerOffenseGameMode> InTimerMethod);
	void SetupAmbientSound();

	UFUNCTION()
	void SetupOnPlayerLosesEndMatchTimer(AActor* Actor);

	void OnEnemySpawned(AActor* Actor);
	void OnEnemyDestroyed(AActor* Actor);
	void OnStartDelay();
	void OnFinishDelay();
	void OnPlayerWins();
	void OnPlayerLoses();

	void CheckSetup() const;
	void CheckAndSetupWinCondition();

	void ToggleSelectedActorsTick(bool bShouldTick) const;
	void DisableSelectedActorsTick();
	void EnableSelectedActorsTick();

	virtual void Tick(float DeltaSeconds) override;
	void BroadcastRemainingTime() const;

public:
	FDelegateHandle AddPlayerWinsHandler(const FPlayerWinsDelegate::FDelegate& Delegate);
	FDelegateHandle AddPlayerLosesHandler(const FPlayerWinsDelegate::FDelegate& Delegate);
	FDelegateHandle AddEnemiesCountChangedHandler(const FOnEnemiesCountChangedDelegate::FDelegate& Delegate);
	FDelegateHandle AddDelayStartHandler(const FOnDelayStartDelegate::FDelegate& Delegate);
	FDelegateHandle AddDelayFinishHandler(const FOnDelayFinishDelegate::FDelegate& Delegate);
	FDelegateHandle AddDelayRemainingTimeHandler(const FOnDelayRemainingTimeDelegate::FDelegate& Delegate);
};
