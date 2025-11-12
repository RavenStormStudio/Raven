// RavenStorm Copyright @ 2025-2025

#include "GameFramework/RavenGameInstanceBase.h"

URavenGameInstanceBase::URavenGameInstanceBase()
{
}

void URavenGameInstanceBase::Init()
{
	Super::Init();
}

void URavenGameInstanceBase::Shutdown()
{
	Super::Shutdown();
}

APlayerController* URavenGameInstanceBase::GetPrimaryPlayerController() const
{
	if (const ULocalPlayer* LocalPlayer = GetPrimaryLocalPlayer())
	{
		return LocalPlayer->GetPlayerController(GetWorld());
	}
	return nullptr;
}

ULocalPlayer* URavenGameInstanceBase::GetPrimaryLocalPlayer() const
{
	return GetLocalPlayerByIndex(0);
}
