#include "TowerPawn.h"

ATowerPawn::ATowerPawn()
{
	HealthBarWidgetComponent = CreateDefaultSubobject<UHealthBarWidgetComponent>(TEXT("HealthBarWidgetComponent"));
	HealthBarWidgetComponent->SetupAttachment(RootComponent);
}

void ATowerPawn::RotateByYaw(const float Yaw)
{
	if (!IsValid(TurretMesh)) return;

	const FRotator CurrentRotation = TurretMesh->GetComponentRotation();
	FRotator NewRotation = CurrentRotation;
	NewRotation.Yaw += Yaw;

	TurretMesh->SetWorldRotation(NewRotation);
}

void ATowerPawn::RefreshHealthBarVisibility()
{
	if (!IsValid(HealthBarWidgetComponent)) return;

	const bool bIsVisibleToPlayer = IsVisibleToPlayer();
	if (HealthBarWidgetComponent->IsVisible() != bIsVisibleToPlayer)
	{
		HealthBarWidgetComponent->SetVisibility(bIsVisibleToPlayer);
	}
}

bool ATowerPawn::IsVisibleToPlayer() const
{
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return false;

	const APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!IsValid(PlayerController)) return false;

	const APawn* PlayerPawn = PlayerController->GetPawn();
	if (!IsValid(PlayerPawn)) return false;

	FHitResult HitResult;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);
	World->LineTraceSingleByChannel(HitResult, GetActorLocation(), PlayerPawn->GetActorLocation(), ECC_Visibility, CollisionQueryParams);

	return HitResult.GetActor() == PlayerPawn;
}

void ATowerPawn::BeginPlay()
{
	Super::BeginPlay();

	RefreshHealthBarVisibility();
}

void ATowerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RefreshHealthBarVisibility();
}

FVector ATowerPawn::GetRelativeProjectileSpawnLocation() const
{
	if (!IsValid(ProjectileSpawnPoint) || !IsValid(TurretMesh)) return FVector::ZeroVector;

	const FRotator TurretMeshRotation = TurretMesh->GetRelativeRotation();
	return TurretMeshRotation.RotateVector(ProjectileSpawnPoint->GetRelativeLocation());
}

FRotator ATowerPawn::GetRelativeTurretMeshRotation() const
{
	if (!IsValid(TurretMesh)) return FRotator::ZeroRotator;

	return TurretMesh->GetRelativeRotation();
}