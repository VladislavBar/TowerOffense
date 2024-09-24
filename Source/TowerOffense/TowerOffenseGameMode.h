#pragma once

#include "CoreMinimal.h"
#include "TurretPawn.h"
#include "GameFramework/GameModeBase.h"
#include "TowerOffenseGameMode.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTowerOffenseGameMode, Log, All);

DECLARE_MULTICAST_DELEGATE(FPlayerWinsDelegate);
DECLARE_MULTICAST_DELEGATE(FPlayerLosesDelegate);

UCLASS()
class TOWEROFFENSE_API ATowerOffenseGameMode : public AGameModeBase
{
	GENERATED_BODY()

	int32 EnemyCount = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy")
	TSubclassOf<ATurretPawn> EnemyClass;

	FOnActorSpawned::FDelegate OnEnemySpawnedDelegate;
	FOnActorDestroyed::FDelegate OnEnemyDestroyedDelegate;
	FPlayerWinsDelegate PlayerWinsDelegate;
	FPlayerLosesDelegate PlayerLosesDelegate;

	FDelegateHandle OnEnemySpawnedDelegateHandle;
	FDelegateHandle OnEnemyDestroyedDelegateHandle;

	virtual void BeginPlay() override;

	void SetupEnemyCount();
	void SetupDelegates();
	void SetupOnEnemySpawnedDelegate();
	void SetupOnEnemyDestroyedDelegate();

	void OnEnemySpawned(AActor* Actor);
	void OnEnemyDestroyed(AActor* Actor);

	void CheckSetup() const;
	void CheckWinCondition() const;

public:
	FDelegateHandle AddPlayerWinsHandler(const FPlayerWinsDelegate::FDelegate& Delegate);
};
