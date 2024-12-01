#pragma once

#include "CoreMinimal.h"
#include "TurretPawn.h"

class TOWEROFFENSE_API FTeamHelper
{
public:
	static int32 GetEnemiesCount(const TArray<ATurretPawn*>& Participants, const ETeam Team);
	static TArray<ATurretPawn*> GetEnemies(const TArray<ATurretPawn*>& Participants, const ETeam Team);
	static TArray<ETeam> GetTeams(const TArray<ATurretPawn*>& Participants);
};
