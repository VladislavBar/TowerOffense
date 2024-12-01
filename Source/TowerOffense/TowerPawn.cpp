#include "TowerPawn.h"

ATowerPawn::ATowerPawn()
{
	SetTeam(ETeam::Towers);
}

void ATowerPawn::BeginPlay()
{
	Super::BeginPlay();
	SetTeam(ETeam::Towers);
}
