#pragma once

#include "Components/CapsuleComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "TurretPawn.generated.h"

UCLASS()
class TOWEROFFENSE_API ATurretPawn : public APawn
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TObjectPtr<UCapsuleComponent> NewRootComponent;

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> BaseMesh;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> TurretMesh;

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<USceneComponent> ProjectileSpawnPoint;

	UPROPERTY(EditAnywhere, meta = (GetOptions = "GetMaterialTeamColorSlotNames"))
	FName MaterialTeamColorSlotName;

	UPROPERTY(EditAnywhere)
	FName MaterialTeamColorParameterName;

	UPROPERTY(EditAnywhere)
	FColor TeamColor;

	UFUNCTION()
	TArray<FName> GetMaterialTeamColorSlotNames() const;

	FVector TargetLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Turret Rotation", meta = (ClampMin = "0.0"))
	float RotationSpeed = 2.f;
	float MeshDefaultRotationYaw = 90.f;

protected:
	void SetTargetLocation(const FVector& Location);
	void RotateTurretMesh(const float DeltaSeconds);

private:
	void SetupTeamColorDynamicMaterial(UStaticMeshComponent* Mesh);
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaSeconds) override;

public:
	ATurretPawn();
};
