// RavenStorm Copyright @ 2025-2025

#include "Pool/RavenPoolDeveloperSettings.h"

FName URavenPoolDeveloperSettings::GetContainerName() const
{
	return TEXT("Project");
}

FName URavenPoolDeveloperSettings::GetCategoryName() const
{
	return TEXT("Raven");
}

const TArray<FRavenPoolConfig>& URavenPoolDeveloperSettings::GetPoolConfigs() const
{
	return PoolConfigs;
}
