#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TowerOffenseGameState.h"

#include "TurretPawn.h"
#include "Components/StateTreeComponent.h"
#include "Perception/AISenseConfig.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Team.h"
#include "Perception/AISense_Sight.h"
#include "EnemyTowerAIController.generated.h"

UCLASS()
class TOWEROFFENSE_API AEnemyTowerAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyTowerAIController();

private:
	UPROPERTY(EditDefaultsOnly, Category = "State Tree")
	TObjectPtr<UStateTreeComponent> StateTreeComponent;

	UPROPERTY(EditDefaultsOnly, Category = "State Tree")
	FStateTreeEvent ResetEnemiesEvent;

	FOnActorTickableEnabledDelegate::FDelegate OnActorTickableEnabledDelegate;
	FDelegateHandle OnPossessedPawnTickableEnabledDelegateHandle;

	FOnParticipantsAmountChangedDelegate::FDelegate OnParticipantAmountChangedDelegate;
	FDelegateHandle OnPlayersAmountChangedDelegateHandle;

	bool bCanRunStateTree = false;

public:
	virtual void SetActorTickEnabled(bool bEnabled) override;

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

private:
	void RunStateTree() const;
	void StopStateTree(const FString& Reason) const;
	void SyncStateTreeToTickableState() const;

	void SubscribeToPlayersAmountChangedEvent();
	void UnsubscribeFromPLayersAmountChangedEvent() const;

	void SetupOnPossessedPawnTickableDelegate();
	void ClearOnPossessedPawnTickableDelegate();

	void OnPossessedPawnTickableEnabled(const bool bEnabled) const;
	void OnParticipantsAmountChanged(const TArray<ATurretPawn*>& Players) const;
};
