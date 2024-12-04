#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "SkipOwnerReplSceneComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TOWEROFFENSE_API USkipOwnerReplSceneComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
