#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "CustomReplicationCapsuleComp.generated.h"

UCLASS()
class TOWEROFFENSE_API UCustomReplicationCapsuleComp : public UCapsuleComponent
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
