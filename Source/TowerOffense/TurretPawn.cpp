#include "TurretPawn.h"


ATurretPawn::ATurretPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	NewRootComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RootComponent"));
	RootComponent = NewRootComponent;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMesh->SetupAttachment(RootComponent);

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretMesh"));
	TurretMesh->SetupAttachment(RootComponent);

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSpawnPoint"));
	ProjectileSpawnPoint->SetupAttachment(RootComponent);
}


TArray<FName> ATurretPawn::GetMaterialTeamColorSlotNames() const
{
	TSet<FName> MaterialNames;
	MaterialNames.Append(BaseMesh->GetMaterialSlotNames());
	MaterialNames.Append(TurretMesh->GetMaterialSlotNames());
	return MaterialNames.Array();
}

void ATurretPawn::SetupTeamColorDynamicMaterial(UStaticMeshComponent* Mesh)
{
	if (!IsValid(Mesh) || MaterialTeamColorSlotName.IsNone())return;

	const int32 MaterialIndex = Mesh->GetMaterialIndex(MaterialTeamColorSlotName);
	if (MaterialIndex == INDEX_NONE)return;

	UMaterialInterface* Material = Mesh->GetMaterial(MaterialIndex);
	if (!IsValid(Material))return;

	UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(Material, nullptr);
	DynamicMaterial->SetVectorParameterValue(MaterialTeamColorParameterName, TeamColor);

	Mesh->SetMaterial(MaterialIndex, DynamicMaterial);
}

void ATurretPawn::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	SetupTeamColorDynamicMaterial(BaseMesh);
	SetupTeamColorDynamicMaterial(TurretMesh);
}
