#pragma once

#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "CoreMinimal.h"

#include "BTService_SetupEnemyTowerSetupBlackboard.generated.h"

UCLASS()
class TOWEROFFENSE_API UBTService_SetupEnemyTowerBlackboard : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTService_SetupEnemyTowerBlackboard();

private:
	virtual void OnSearchStart(FBehaviorTreeSearchData& SearchData) override;
	AActor* GetPlayerPawn() const;
	virtual FString GetStaticDescription() const override;
};
