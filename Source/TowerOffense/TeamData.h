#pragma once

#include "CoreMinimal.h"
#include "TurretPawn.h"
#include "UObject/NoExportTypes.h"
#include "TeamData.generated.h"

USTRUCT()
struct TOWEROFFENSE_API FTeamData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	ETeam Team;

	UPROPERTY(EditAnywhere)
	FLinearColor Color;
};
