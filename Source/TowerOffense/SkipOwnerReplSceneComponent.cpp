#include "SkipOwnerReplSceneComponent.h"

#include "Net/UnrealNetwork.h"

void USkipOwnerReplSceneComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Done similarly to DISABLE_REPLICATED_PRIVATE_PROPERTY
	ResetReplicatedLifetimeProperty(
		StaticClass(), USkipOwnerReplSceneComponent::StaticClass(), FName(TEXT("RelativeLocation")), COND_SkipOwner, OutLifetimeProps);
	ResetReplicatedLifetimeProperty(
		StaticClass(), USkipOwnerReplSceneComponent::StaticClass(), FName(TEXT("RelativeRotation")), COND_SkipOwner, OutLifetimeProps);
}