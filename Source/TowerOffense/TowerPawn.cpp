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
	if (!IsValid(ProjectileSpawnPoint) || !IsValid(TurretMesh)) return FVector::ZeroVector;

	const FRotator TurretMeshRotation = TurretMesh->GetRelativeRotation();
	return TurretMeshRotation.RotateVector(ProjectileSpawnPoint->GetRelativeLocation());
}

FRotator ATowerPawn::GetRelativeTurretMeshRotation() const
{
	if(!IsValid(TurretMesh)) return FRotator::ZeroRotator;
	
	return TurretMesh->GetRelativeRotation();
}