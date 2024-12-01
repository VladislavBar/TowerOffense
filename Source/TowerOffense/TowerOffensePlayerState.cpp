#include "TowerOffensePlayerState.h"
#include "Net/UnrealNetwork.h"

ATowerOffensePlayerState::ATowerOffensePlayerState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATowerOffensePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATowerOffensePlayerState, Team);
}

void ATowerOffensePlayerState::SetTeam(ETeam NewTeam)
{
	Team = NewTeam;
}