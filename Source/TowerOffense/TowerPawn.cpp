#include "TowerPawn.h"

ATowerPawn::ATowerPawn()
{
	HealthBarWidgetComponent = CreateDefaultSubobject<UHealthBarWidgetComponent>(TEXT("HealthBarWidgetComponent"));
	HealthBarWidgetComponent->SetupAttachment(RootComponent);
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
