#include "CustomReplicationCapsuleComp.h"

#include "Net/UnrealNetwork.h"

void UCustomReplicationCapsuleComp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Done similarly to DISABLE_REPLICATED_PRIVATE_PROPERTY
	ResetReplicatedLifetimeProperty(
		StaticClass(), UCustomReplicationCapsuleComp::StaticClass(), FName(TEXT("RelativeLocation")), COND_SkipOwner, OutLifetimeProps);
	ResetReplicatedLifetimeProperty(
		StaticClass(), UCustomReplicationCapsuleComp::StaticClass(), FName(TEXT("RelativeRotation")), COND_SkipOwner, OutLifetimeProps);
}