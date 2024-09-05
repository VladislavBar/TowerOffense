#include "TurretPawn.h"

#include "Kismet/KismetMathLibrary.h"

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

	if (IsValid(BaseMesh))
	{
		MaterialNames.Append(BaseMesh->GetMaterialSlotNames());
	}

	if (IsValid(TurretMesh))
	{
		MaterialNames.Append(TurretMesh->GetMaterialSlotNames());
	}

	return MaterialNames.Array();
}

void ATurretPawn::RotateTurretMesh(const float DeltaSeconds)
{
	if (!bLockTarget || !IsValid(TurretMesh)) return;
	return RotateTurretMeshToLocation(DeltaSeconds, TargetLocation);
}

void ATurretPawn::RotateTurretMeshToLocation(const float DeltaSeconds, const FVector& Location)
{
	const FRotator CurrentRotation = TurretMesh->GetComponentRotation();
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Location);

	// That is needed in case when a TurretMesh is aligned by default in a different direction than the x-axis
	// In the mesh provided, the turret mesh is aligned by default by the y-axis...
	TargetRotation.Yaw -= MeshDefaultRotationYaw;

	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaSeconds, RotationSpeed);
	NewRotation.Roll = 0.f;
	NewRotation.Pitch = 0.f;

	TurretMesh->SetWorldRotation(NewRotation);
}

void ATurretPawn::SetTargetLocation(const FVector& Location)
{
	TargetLocation = Location;
}

void ATurretPawn::SetupTeamColorDynamicMaterial(UStaticMeshComponent* Mesh)
{
	if (!IsValid(Mesh) || MaterialTeamColorSlotName.IsNone()) return;

	const int32 MaterialIndex = Mesh->GetMaterialIndex(MaterialTeamColorSlotName);
	if (MaterialIndex == INDEX_NONE) return;

	UMaterialInterface* Material = Mesh->GetMaterial(MaterialIndex);
	if (!IsValid(Material)) return;

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

void ATurretPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	RotateTurretMesh(DeltaSeconds);
}
