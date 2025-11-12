// RavenStorm Copyright @ 2025-2025

#pragma once

#include "CoreMinimal.h"
#include "Pool/RavenPoolTypes.h"
#include "RavenPool.generated.h"

class URavenPoolFactoryUObject;
class IRavenPoolAcquisitionStrategy;

/**
 * Represents a single entry in the object pool.
 * Tracks whether the object is currently active (in use) or inactive (available for reuse).
 */
USTRUCT(BlueprintType)
struct RAVEN_API FRavenPoolEntry
{
	GENERATED_BODY()

	/** Whether this pooled object is currently active/in use */
	UPROPERTY()
	bool bIsActive = false;

	/** The pooled object instance */
	UPROPERTY()
	TObjectPtr<UObject> Object = nullptr;

	/** Last time this object was used (for LRU strategy) */
	UPROPERTY()
	double LastUsedTime = 0.0;

	/** Number of times this object has been acquired */
	UPROPERTY()
	int32 AcquireCount = 0;

	/**
	 * Validates that the object is still valid for use.
	 * @return True if valid, false otherwise
	 */
	bool Validate() const;

public:
	friend RAVEN_API bool operator==(const FRavenPoolEntry& A, const UObject* B) { return A.Object == B; }
};

/**
 * A pool for managing reusable objects of a specific class.
 * Reduces allocation overhead by reusing objects instead of creating and destroying them.
 */
USTRUCT(BlueprintType)
struct RAVEN_API FRavenPool
{
	GENERATED_BODY()

public:
	FRavenPool() = default;
	explicit FRavenPool(UClass* InObjectClass);

public:
	/**
	 * Acquires an object from the pool.
	 * Reuses an inactive object if available, otherwise creates a new one.
	 * @return The acquired object, or nullptr if acquisition fails
	 */
	UObject* Acquire();

	/**
	 * Releases an object back to the pool for reuse.
	 * @param Object The object to release
	 * @return True if the object was successfully released, false otherwise
	 */
	bool Release(UObject* Object);

	/**
	 * Pre-warms the pool by creating a specified number of objects in advance.
	 * @param Count The number of objects to pre-create
	 */
	void PreWarm(int32 Count);

	/**
	 * Pre-warms the pool asynchronously (not yet implemented - placeholder).
	 * @param Count The number of objects to pre-create
	 * @param Callback Called when pre-warming completes
	 */
	void PreWarmAsync(int32 Count, TFunction<void()> Callback = nullptr);

	/**
	 * Clears all inactive objects from the pool.
	 * Active objects will remain until released.
	 */
	void ClearInactive();

	/**
	 * Performs periodic maintenance on the pool based on policy settings.
	 * @param DeltaTime Time since last tick
	 */
	void Tick(float DeltaTime);

	/**
	 * Validates all pooled objects and removes invalid ones.
	 * @return Number of invalid objects removed
	 */
	int32 ValidatePool();

	/**
	 * Gets the total number of objects in the pool (active and inactive).
	 * @return The pool size
	 */
	int32 GetPoolSize() const { return Pool.Num(); }

	/**
	 * Gets the number of active (in-use) objects in the pool.
	 * @return The number of active objects
	 */
	int32 GetActiveCount() const;

	/**
	 * Gets the number of inactive (available) objects in the pool.
	 * @return The number of inactive objects
	 */
	int32 GetInactiveCount() const;

	/**
	 * Gets detailed statistics for this pool.
	 * @return Pool statistics
	 */
	FRavenPoolStats GetStats() const;

	/**
	 * Gets the class of objects managed by this pool.
	 * @return The object class
	 */
	UClass* GetObjectClass() const { return ObjectClass; }

	/**
	 * Gets the maximum allowed pool size (0 = unlimited).
	 * @return The maximum pool size
	 */
	int32 GetMaxPoolSize() const { return MaxPoolSize; }

	/**
	 * Sets the maximum allowed pool size (0 = unlimited).
	 * @param InMaxSize The new maximum pool size
	 */
	void SetMaxPoolSize(int32 InMaxSize) { MaxPoolSize = InMaxSize; }

	/**
	 * Gets the pool policy configuration.
	 * @return The pool policy
	 */
	const FRavenPoolPolicy& GetPolicy() const { return Policy; }

	/**
	 * Sets the pool policy configuration.
	 * @param InPolicy The new pool policy
	 */
	void SetPolicy(const FRavenPoolPolicy& InPolicy);

private:
	/**
	 * Finds an inactive object using the configured acquisition strategy.
	 * @return Index of inactive object, or INDEX_NONE if none available
	 */
	int32 FindInactiveObject();

	/**
	 * Rebuilds the inactive indices cache.
	 */
	void RebuildInactiveIndices();

	/**
	 * Marks statistics as dirty for recalculation.
	 */
	void MarkStatsDirty() { bStatsDirty = true; }

private:
	/** Array of pooled objects */
	UPROPERTY()
	TArray<FRavenPoolEntry> Pool;

	/** The class of objects this pool manages */
	UPROPERTY()
	TObjectPtr<UClass> ObjectClass = nullptr;

	/** Factory used to create and prepare pooled objects */
	UPROPERTY()
	TObjectPtr<URavenPoolFactoryUObject> Factory;

	/** Maximum number of objects allowed in the pool (0 = unlimited) */
	UPROPERTY()
	int32 MaxPoolSize = 0;

	/** Pool management policy */
	UPROPERTY()
	FRavenPoolPolicy Policy;

	/** Cached indices of inactive objects for fast lookup */
	TArray<int32> InactiveIndices;

	/** Map from object to pool index for fast reverse lookup */
	TMap<TObjectPtr<UObject>, int32> ObjectToIndex;

	/** Whether the inactive indices cache is valid */
	bool bInactiveIndicesDirty = true;

	/** Acquisition strategy for selecting objects from pool */
	TSharedPtr<IRavenPoolAcquisitionStrategy> AcquisitionStrategy;

	/** Cached statistics */
	mutable FRavenPoolStats CachedStats;

	/** Whether statistics need to be recalculated */
	mutable bool bStatsDirty = true;

	/** Time since last shrink operation */
	float TimeSinceLastShrink = 0.0f;

	friend class RAVEN_API URavenPoolSubsystem;

public:
	friend RAVEN_API bool operator==(const FRavenPool& A, const UClass* B) { return A.ObjectClass == B; }
};
