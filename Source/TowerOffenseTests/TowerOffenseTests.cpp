#include "TowerOffenseTests.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_MODULE(FTowerOffenseTestsModule, TowerOffenseTests);

#include "Misc/AutomationTest.h"
#include "TeamHelper.h"
#include "TurretPawn.h"

UCLASS()
class ATurretPawnTestable : public ATurretPawn
{
    GENERATED_BODY()
public:
    void SetTeamForTest(ETeam NewTeam)
    {
        SetTeam(NewTeam);
    }
};

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTeamHelperEnemiesCountTest, "TowerOffense.TeamHelper.GetEnemiesCount", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTeamHelperEnemiesCountTest::RunTest(const FString& Parameters)
{
    TArray<ATurretPawn*> Participants;

    ATurretPawnTestable* Pawn1 = NewObject<ATurretPawnTestable>();
    Pawn1->SetTeamForTest(ETeam::Team1);
    Participants.Add(Pawn1);

    ATurretPawnTestable* Pawn2 = NewObject<ATurretPawnTestable>();
    Pawn2->SetTeamForTest(ETeam::Team2);
    Participants.Add(Pawn2);

    ATurretPawnTestable* Pawn3 = NewObject<ATurretPawnTestable>();
    Pawn3->SetTeamForTest(ETeam::Team2);
    Participants.Add(Pawn3);

    TestEqual(TEXT("Team1 enemies"), FTeamHelper::GetEnemiesCount(Participants, ETeam::Team1), 2);
    TestEqual(TEXT("Team2 enemies"), FTeamHelper::GetEnemiesCount(Participants, ETeam::Team2), 1);

    Participants.Add(nullptr);
    TestEqual(TEXT("Team1 enemies with null"), FTeamHelper::GetEnemiesCount(Participants, ETeam::Team1), 2);

    return true;
}

