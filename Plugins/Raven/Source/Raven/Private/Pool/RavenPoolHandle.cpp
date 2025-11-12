// RavenStorm Copyright @ 2025-2025

#include "Pool/RavenPoolHandle.h"
#include "Pool/RavenPoolSubsystem.h"

void FRavenPoolHandle::Release()
{
	if (Object.IsValid() && Subsystem.IsValid())
	{
		Subsystem->Release(Object.Get());
	}
	bAutoRelease = false;
}
