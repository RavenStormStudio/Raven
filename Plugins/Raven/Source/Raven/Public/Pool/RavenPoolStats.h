// RavenStorm Copyright @ 2025-2025

#pragma once

#include "CoreMinimal.h"
#include "Stats/Stats.h"

/**
 * Profiling statistics for the RavenPool system.
 * All pool-related performance stats are organized under STATGROUP_RavenPool.
 *
 * Usage in console:
 *   stat RavenPool     - Display all pool statistics
 *   stat startfile     - Start recording stats to file
 *   stat stopfile      - Stop recording stats
 *
 * Note: Stats are compiled out in Shipping builds for zero overhead.
 */

// Stat group for all pool-related statistics
DECLARE_STATS_GROUP(TEXT("RavenPool"), STATGROUP_RavenPool, STATCAT_Advanced);

// ============================================================================
// Core Pool Statistics (FRavenPool)
// ============================================================================

/** Time spent acquiring objects from the pool */
DECLARE_CYCLE_STAT_EXTERN(TEXT("Pool Acquire"), STAT_Pool_Acquire, STATGROUP_RavenPool, RAVEN_API);

/** Time spent releasing objects back to the pool */
DECLARE_CYCLE_STAT_EXTERN(TEXT("Pool Release"), STAT_Pool_Release, STATGROUP_RavenPool, RAVEN_API);

/** Time spent pre-warming the pool with objects */
DECLARE_CYCLE_STAT_EXTERN(TEXT("Pool PreWarm"), STAT_Pool_PreWarm, STATGROUP_RavenPool, RAVEN_API);

/** Time spent clearing inactive objects from the pool */
DECLARE_CYCLE_STAT_EXTERN(TEXT("Pool ClearInactive"), STAT_Pool_ClearInactive, STATGROUP_RavenPool, RAVEN_API);

/** Time spent on pool maintenance per tick */
DECLARE_CYCLE_STAT_EXTERN(TEXT("Pool Tick"), STAT_Pool_Tick, STATGROUP_RavenPool, RAVEN_API);

/** Time spent validating pooled objects */
DECLARE_CYCLE_STAT_EXTERN(TEXT("Pool Validate"), STAT_Pool_Validate, STATGROUP_RavenPool, RAVEN_API);

/** Time spent rebuilding inactive object indices cache */
DECLARE_CYCLE_STAT_EXTERN(TEXT("Pool RebuildIndices"), STAT_Pool_RebuildIndices, STATGROUP_RavenPool, RAVEN_API);

/** Time spent finding inactive objects using acquisition strategy */
DECLARE_CYCLE_STAT_EXTERN(TEXT("Pool FindInactive"), STAT_Pool_FindInactive, STATGROUP_RavenPool, RAVEN_API);

// ============================================================================
// Subsystem Statistics (URavenPoolSubsystem)
// ============================================================================

/** Total time to acquire object including pool lookup */
DECLARE_CYCLE_STAT_EXTERN(TEXT("Subsystem Acquire"), STAT_PoolSubsystem_Acquire, STATGROUP_RavenPool, RAVEN_API);

/** Total time to release object including pool lookup */
DECLARE_CYCLE_STAT_EXTERN(TEXT("Subsystem Release"), STAT_PoolSubsystem_Release, STATGROUP_RavenPool, RAVEN_API);

/** Time spent ticking all pools for maintenance */
DECLARE_CYCLE_STAT_EXTERN(TEXT("Subsystem Tick"), STAT_PoolSubsystem_Tick, STATGROUP_RavenPool, RAVEN_API);

/** Time spent initializing the pool subsystem */
DECLARE_CYCLE_STAT_EXTERN(TEXT("Subsystem Initialize"), STAT_PoolSubsystem_Initialize, STATGROUP_RavenPool, RAVEN_API);

/** Time spent getting or creating a pool for a class */
DECLARE_CYCLE_STAT_EXTERN(TEXT("Subsystem GetPool"), STAT_PoolSubsystem_GetPool, STATGROUP_RavenPool, RAVEN_API);

// ============================================================================
// Factory Statistics (URavenPoolFactoryUObject)
// ============================================================================

/** Time spent creating new pooled objects */
DECLARE_CYCLE_STAT_EXTERN(TEXT("Factory Create"), STAT_Factory_Create, STATGROUP_RavenPool, RAVEN_API);

/** Time spent destroying pooled objects */
DECLARE_CYCLE_STAT_EXTERN(TEXT("Factory Destroy"), STAT_Factory_Destroy, STATGROUP_RavenPool, RAVEN_API);

/** Time spent preparing objects for storage (when released) */
DECLARE_CYCLE_STAT_EXTERN(TEXT("Factory PrepareStorage"), STAT_Factory_PrepareStorage, STATGROUP_RavenPool, RAVEN_API);

/** Time spent preparing objects for usage (when acquired) */
DECLARE_CYCLE_STAT_EXTERN(TEXT("Factory PrepareUsage"), STAT_Factory_PrepareUsage, STATGROUP_RavenPool, RAVEN_API);

// ============================================================================
// Actor Factory Statistics (URavenPoolActorFactory)
// ============================================================================

/** Time spent spawning pooled actors */
DECLARE_CYCLE_STAT_EXTERN(TEXT("Actor Factory Create"), STAT_ActorFactory_Create, STATGROUP_RavenPool, RAVEN_API);

/** Time spent destroying pooled actors */
DECLARE_CYCLE_STAT_EXTERN(TEXT("Actor Factory Destroy"), STAT_ActorFactory_Destroy, STATGROUP_RavenPool, RAVEN_API);

/** Time spent preparing actors for storage (disable/deactivate) */
DECLARE_CYCLE_STAT_EXTERN(TEXT("Actor Factory PrepareStorage"), STAT_ActorFactory_PrepareStorage, STATGROUP_RavenPool, RAVEN_API);

/** Time spent preparing actors for usage (enable/activate) */
DECLARE_CYCLE_STAT_EXTERN(TEXT("Actor Factory PrepareUsage"), STAT_ActorFactory_PrepareUsage, STATGROUP_RavenPool, RAVEN_API);
