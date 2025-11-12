// RavenStorm Copyright @ 2025-2025

#pragma once

#include "CoreMinimal.h"
#include "Pool/RavenPoolTypes.h"

struct FRavenPoolEntry;

/**
 * Base interface for pool acquisition strategies.
 * Determines how objects are selected from the pool when acquired.
 */
class RAVEN_API IRavenPoolAcquisitionStrategy
{
public:
	virtual ~IRavenPoolAcquisitionStrategy() = default;

	/**
	 * Finds an inactive object from the pool.
	 * @param Pool The pool entries to search
	 * @param InactiveIndices Pre-computed indices of inactive entries for faster lookup
	 * @return Index of the selected entry, or INDEX_NONE if none available
	 */
	virtual int32 FindInactiveObject(const TArray<FRavenPoolEntry>& Pool, const TArray<int32>& InactiveIndices) = 0;

	/**
	 * Called when an object is acquired, allowing the strategy to update its state.
	 * @param Index The index of the acquired object
	 */
	virtual void OnObjectAcquired(int32 Index) {}

	/**
	 * Called when an object is released, allowing the strategy to update its state.
	 * @param Index The index of the released object
	 */
	virtual void OnObjectReleased(int32 Index) {}
};

/**
 * FIFO (First-In-First-Out) acquisition strategy.
 * Reuses the oldest inactive object.
 */
class RAVEN_API FRavenPoolFIFOStrategy : public IRavenPoolAcquisitionStrategy
{
public:
	virtual int32 FindInactiveObject(const TArray<FRavenPoolEntry>& Pool, const TArray<int32>& InactiveIndices) override;
};

/**
 * LIFO (Last-In-First-Out) acquisition strategy.
 * Reuses the most recently released object.
 */
class RAVEN_API FRavenPoolLIFOStrategy : public IRavenPoolAcquisitionStrategy
{
public:
	virtual int32 FindInactiveObject(const TArray<FRavenPoolEntry>& Pool, const TArray<int32>& InactiveIndices) override;
};

/**
 * LRU (Least Recently Used) acquisition strategy.
 * Reuses the object that hasn't been used for the longest time.
 */
class RAVEN_API FRavenPoolLRUStrategy : public IRavenPoolAcquisitionStrategy
{
public:
	virtual int32 FindInactiveObject(const TArray<FRavenPoolEntry>& Pool, const TArray<int32>& InactiveIndices) override;
};

/**
 * Random acquisition strategy.
 * Selects a random inactive object from the pool.
 */
class RAVEN_API FRavenPoolRandomStrategy : public IRavenPoolAcquisitionStrategy
{
public:
	virtual int32 FindInactiveObject(const TArray<FRavenPoolEntry>& Pool, const TArray<int32>& InactiveIndices) override;
};

/**
 * Factory for creating acquisition strategies.
 */
class RAVEN_API FRavenPoolStrategyFactory
{
public:
	/**
	 * Creates an acquisition strategy based on the specified type.
	 * @param StrategyType The type of strategy to create
	 * @return The created strategy, or nullptr if invalid type
	 */
	static TSharedPtr<IRavenPoolAcquisitionStrategy> CreateStrategy(ERavenPoolAcquisitionStrategy StrategyType);
};
