#include "TowerPawn.h"

void ATowerPawn::RotateByYaw(const float Yaw)
{
	if (!IsValid(TurretMesh)) return;

	const FRotator CurrentRotation = TurretMesh->GetComponentRotation();
	FRotator NewRotation = CurrentRotation;
	NewRotation.Yaw += Yaw;

	TurretMesh->SetWorldRotation(NewRotation);
}

FVector ATowerPawn::GetRelativeProjectileSpawnLocation() const
{
	if (!IsValid(ProjectileSpawnPoint)) return FVector::ZeroVector;

	const FRotator TurretMeshRotation = TurretMesh->GetRelativeRotation();
	return TurretMeshRotation.RotateVector(ProjectileSpawnPoint->GetRelativeLocation());
}

FRotator ATowerPawn::GetRelativeTurretMeshRotation() const
{
	return TurretMesh->GetRelativeRotation();
}