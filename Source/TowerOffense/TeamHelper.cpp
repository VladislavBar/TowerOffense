#include "TeamHelper.h"

int32 FTeamHelper::GetEnemiesCount(const TArray<ATurretPawn*>& Participants, const ETeam Team)
{
	return GetEnemies(Participants, Team).Num();
}

TArray<ATurretPawn*> FTeamHelper::GetEnemies(const TArray<ATurretPawn*>& Participants, const ETeam Team)
{
	return Participants.FilterByPredicate([Team](const ATurretPawn* Participant) {
		if (!IsValid(Participant)) return false;
		return Participant->GetTeam() != Team;
	});
}

TArray<ETeam> FTeamHelper::GetTeams(const TArray<ATurretPawn*>& Participants)
{
	TMap<ETeam, int32> TeamsCount;
	for (const ATurretPawn* Participant : Participants)
	{
		if (!IsValid(Participant)) continue;

		const ETeam Team = Participant->GetTeam();
		if (TeamsCount.Contains(Team))
		{
			TeamsCount[Team]++;
		}
		else
		{
			TeamsCount.Add(Team, 1);
		}
	}

	TArray<ETeam> Teams;
	TeamsCount.GetKeys(Teams);
	return Teams;
}