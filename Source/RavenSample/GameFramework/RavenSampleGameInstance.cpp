// RavenStorm Copyright @ 2025-2025

#include "RavenSampleGameInstance.h"

#include "Pool/RavenPoolSubsystem.h"

AActor* URavenSampleGameInstance::AcquireActor(UClass* Class)
{
	return Cast<AActor>(GetWorld()->GetSubsystem<URavenPoolSubsystem>()->Acquire(Class));
}
