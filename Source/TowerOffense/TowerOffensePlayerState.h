#pragma once

#include "CoreMinimal.h"
#include "TurretPawn.h"
#include "GameFramework/PlayerState.h"
#include "TowerOffensePlayerState.generated.h"

UCLASS()
class TOWEROFFENSE_API ATowerOffensePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ATowerOffensePlayerState();

private:
	// Should be synchronized with TurretPawn.h, we need this in case a player has died and we need to know which team they were on
	UPROPERTY(Replicated)
	ETeam Team;

private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	ETeam GetTeam() const { return Team; }
	void SetTeam(ETeam NewTeam);
};
