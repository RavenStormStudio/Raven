// RavenStorm Copyright @ 2025-2025

#include "Pool/RavenPoolStats.h"

// Define all cycle stats
DEFINE_STAT(STAT_Pool_Acquire);
DEFINE_STAT(STAT_Pool_Release);
DEFINE_STAT(STAT_Pool_PreWarm);
DEFINE_STAT(STAT_Pool_ClearInactive);
DEFINE_STAT(STAT_Pool_Tick);
DEFINE_STAT(STAT_Pool_Validate);
DEFINE_STAT(STAT_Pool_RebuildIndices);
DEFINE_STAT(STAT_Pool_FindInactive);

DEFINE_STAT(STAT_PoolSubsystem_Acquire);
DEFINE_STAT(STAT_PoolSubsystem_Release);
DEFINE_STAT(STAT_PoolSubsystem_Tick);
DEFINE_STAT(STAT_PoolSubsystem_Initialize);
DEFINE_STAT(STAT_PoolSubsystem_GetPool);

DEFINE_STAT(STAT_Factory_Create);
DEFINE_STAT(STAT_Factory_Destroy);
DEFINE_STAT(STAT_Factory_PrepareStorage);
DEFINE_STAT(STAT_Factory_PrepareUsage);

DEFINE_STAT(STAT_ActorFactory_Create);
DEFINE_STAT(STAT_ActorFactory_Destroy);
DEFINE_STAT(STAT_ActorFactory_PrepareStorage);
DEFINE_STAT(STAT_ActorFactory_PrepareUsage);
