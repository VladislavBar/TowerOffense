#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ActorArrayWrapper.generated.h"

USTRUCT(BlueprintType, DisplayName = "Tank Pawn Array Wrapper")
struct TOWEROFFENSE_API FActorArrayWrapper
{
	GENERATED_BODY()

	FActorArrayWrapper() = default;

	UPROPERTY(VisibleAnywhere)
	TArray<AActor*> Array;
};
