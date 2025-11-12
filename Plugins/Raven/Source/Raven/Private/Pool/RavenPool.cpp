// RavenStorm Copyright @ 2025-2025

#include "Pool/RavenPool.h"
#include "Pool/Factory/RavenPoolFactoryUObject.h"
#include "Pool/Strategy/RavenPoolStrategy.h"
#include "Pool/Interface/Poolable.h"
#include "Pool/RavenPoolStats.h"

DEFINE_LOG_CATEGORY_STATIC(LogRavenPool, Log, All);

bool FRavenPoolEntry::Validate() const
{
	if (!IsValid(Object))
	{
		return false;
	}

	// Check if object implements IPoolable and ask if it's valid for reuse
	if (Object->Implements<UPoolable>())
	{
		IPoolable* Poolable = Cast<IPoolable>(Object);
		if (Poolable && !Poolable->Execute_IsValidForReuse(Object))
		{
			return false;
		}
	}

	return true;
}

FRavenPool::FRavenPool(UClass* InObjectClass)
	: ObjectClass(InObjectClass)
{
	// Create default FIFO strategy
	AcquisitionStrategy = FRavenPoolStrategyFactory::CreateStrategy(ERavenPoolAcquisitionStrategy::FIFO);
}

UObject* FRavenPool::Acquire()
{
	SCOPE_CYCLE_COUNTER(STAT_Pool_Acquire);

	if (!IsValid(Factory) || !IsValid(ObjectClass))
	{
		UE_LOG(LogRavenPool, Error, TEXT("Cannot acquire object: Factory or ObjectClass is invalid"));
		return nullptr;
	}

	// Rebuild inactive indices if needed
	if (bInactiveIndicesDirty)
	{
		RebuildInactiveIndices();
	}

	// Try to reuse an inactive object from the pool
	const int32 InactiveIndex = FindInactiveObject();
	if (InactiveIndex != INDEX_NONE)
	{
		FRavenPoolEntry& Entry = Pool[InactiveIndex];

		// Validate the object before reusing
		if (Policy.bEnableValidation && !Entry.Validate())
		{
			UE_LOG(LogRavenPool, Warning, TEXT("Pooled object failed validation, removing and creating new one"));
			Factory->DestroyPoolObject(Entry.Object);
			Pool.RemoveAtSwap(InactiveIndex);
			bInactiveIndicesDirty = true;
			ObjectToIndex.Remove(Entry.Object);
			MarkStatsDirty();
			return Acquire(); // Recursive call to try again
		}

		Entry.bIsActive = true;
		Entry.LastUsedTime = FPlatformTime::Seconds();
		Entry.AcquireCount++;

		// Remove from inactive indices
		InactiveIndices.Remove(InactiveIndex);

		// Notify strategy
		if (AcquisitionStrategy.IsValid())
		{
			AcquisitionStrategy->OnObjectAcquired(InactiveIndex);
		}

		// Call IPoolable interface
		if (Entry.Object->Implements<UPoolable>())
		{
			IPoolable::Execute_OnAcquiredFromPool(Entry.Object);
		}

		Factory->PrepareForUsage(Entry.Object);

		CachedStats.TotalAcquisitions++;
		CachedStats.TotalReuses++;
		MarkStatsDirty();

		UE_LOG(LogRavenPool, Verbose, TEXT("Reusing pooled object of class %s"), *ObjectClass->GetName());
		return Entry.Object;
	}

	// Check if we've reached the maximum pool size
	if (MaxPoolSize > 0 && Pool.Num() >= MaxPoolSize)
	{
		UE_LOG(LogRavenPool, Warning, TEXT("Cannot create new object: Pool for class %s has reached max size %d"),
			*ObjectClass->GetName(), MaxPoolSize);
		return nullptr;
	}

	// No inactive object found, create a new one
	FPoolCreationContext Context;
	Context.ObjectClass = ObjectClass;
	Context.CurrentPoolSize = Pool.Num();
	Context.bIsPreWarming = false;

	UObject* Object = Factory->CreatePoolObject(ObjectClass);
	if (!IsValid(Object))
	{
		UE_LOG(LogRavenPool, Error, TEXT("Failed to create new pooled object of class %s"), *ObjectClass->GetName());
		return nullptr;
	}

	// Call IPoolable interface
	if (Object->Implements<UPoolable>())
	{
		IPoolable::Execute_OnAcquiredFromPool(Object);
	}

	Factory->PrepareForUsage(Object);

	const int32 NewIndex = Pool.Num();
	FRavenPoolEntry& NewEntry = Pool.Emplace_GetRef(FRavenPoolEntry{
		.bIsActive = true,
		.Object = Object,
		.LastUsedTime = FPlatformTime::Seconds(),
		.AcquireCount = 1
	});

	// Add to object index map
	ObjectToIndex.Add(Object, NewIndex);

	// Update stats
	CachedStats.TotalCreated++;
	CachedStats.TotalAcquisitions++;
	CachedStats.PeakPoolSize = FMath::Max(CachedStats.PeakPoolSize, Pool.Num());
	MarkStatsDirty();

	UE_LOG(LogRavenPool, Log, TEXT("Created new pooled object of class %s (Pool size: %d)"), *ObjectClass->GetName(), Pool.Num());
	return Object;
}

bool FRavenPool::Release(UObject* Object)
{
	SCOPE_CYCLE_COUNTER(STAT_Pool_Release);

	if (!IsValid(Object) || !IsValid(Factory))
	{
		UE_LOG(LogRavenPool, Warning, TEXT("Cannot release object: Object or Factory is invalid"));
		return false;
	}

	// Fast lookup using index map
	int32* IndexPtr = ObjectToIndex.Find(Object);
	if (!IndexPtr || !Pool.IsValidIndex(*IndexPtr))
	{
		UE_LOG(LogRavenPool, Warning, TEXT("Attempted to release object that doesn't belong to this pool"));
		return false;
	}

	FRavenPoolEntry& Entry = Pool[*IndexPtr];

	if (!Entry.bIsActive)
	{
		UE_LOG(LogRavenPool, Warning, TEXT("Attempted to release already inactive object of class %s"), *ObjectClass->GetName());
		return false;
	}

	Entry.bIsActive = false;
	Entry.LastUsedTime = FPlatformTime::Seconds();

	// Mark indices as dirty so they'll be rebuilt on next acquire
	bInactiveIndicesDirty = true;

	// Notify strategy
	if (AcquisitionStrategy.IsValid())
	{
		AcquisitionStrategy->OnObjectReleased(*IndexPtr);
	}

	// Call IPoolable interface
	if (Entry.Object->Implements<UPoolable>())
	{
		IPoolable::Execute_OnReturnedToPool(Entry.Object);
	}

	Factory->PrepareForStorage(Entry.Object);

	CachedStats.TotalReleases++;
	MarkStatsDirty();

	UE_LOG(LogRavenPool, Verbose, TEXT("Released object of class %s back to pool"), *ObjectClass->GetName());
	return true;
}

void FRavenPool::PreWarm(int32 Count)
{
	SCOPE_CYCLE_COUNTER(STAT_Pool_PreWarm);

	if (!IsValid(Factory) || !IsValid(ObjectClass))
	{
		UE_LOG(LogRavenPool, Error, TEXT("Cannot pre-warm pool: Factory or ObjectClass is invalid"));
		return;
	}

	if (Count <= 0)
	{
		return;
	}

	// Calculate how many objects we can actually create based on max pool size
	int32 ObjectsToCreate = Count;
	if (MaxPoolSize > 0)
	{
		const int32 AvailableSlots = MaxPoolSize - Pool.Num();
		ObjectsToCreate = FMath::Min(Count, AvailableSlots);
		if (ObjectsToCreate < Count)
		{
			UE_LOG(LogRavenPool, Warning, TEXT("Pre-warming limited to %d objects (max pool size: %d)"),
				ObjectsToCreate, MaxPoolSize);
		}
	}

	UE_LOG(LogRavenPool, Log, TEXT("Pre-warming pool for class %s with %d objects"),
		*ObjectClass->GetName(), ObjectsToCreate);

	FPoolCreationContext Context;
	Context.ObjectClass = ObjectClass;
	Context.bIsPreWarming = true;

	for (int32 i = 0; i < ObjectsToCreate; ++i)
	{
		Context.CurrentPoolSize = Pool.Num();

		UObject* Object = Factory->CreatePoolObject(ObjectClass);
		if (IsValid(Object))
		{
			Factory->PrepareForStorage(Object);

			const int32 NewIndex = Pool.Num();
			Pool.Emplace(FRavenPoolEntry{
				.bIsActive = false,
				.Object = Object,
				.LastUsedTime = FPlatformTime::Seconds(),
				.AcquireCount = 0
			});

			ObjectToIndex.Add(Object, NewIndex);
			CachedStats.TotalCreated++;
		}
		else
		{
			UE_LOG(LogRavenPool, Error, TEXT("Failed to create object %d/%d during pre-warming"),
				i + 1, ObjectsToCreate);
		}
	}

	bInactiveIndicesDirty = true;
	CachedStats.PeakPoolSize = FMath::Max(CachedStats.PeakPoolSize, Pool.Num());
	MarkStatsDirty();

	UE_LOG(LogRavenPool, Log, TEXT("Pre-warmed pool for class %s (Pool size: %d)"),
		*ObjectClass->GetName(), Pool.Num());
}

void FRavenPool::PreWarmAsync(int32 Count, TFunction<void()> Callback)
{
	// TODO: Implement async pre-warming
	// For now, just call synchronous version
	PreWarm(Count);
	if (Callback)
	{
		Callback();
	}
}

void FRavenPool::ClearInactive()
{
	SCOPE_CYCLE_COUNTER(STAT_Pool_ClearInactive);

	if (!IsValid(Factory))
	{
		UE_LOG(LogRavenPool, Warning, TEXT("Cannot clear inactive objects: Factory is invalid"));
		return;
	}

	const int32 InitialSize = Pool.Num();

	// Destroy all inactive objects
	for (int32 i = Pool.Num() - 1; i >= 0; --i)
	{
		if (!Pool[i].bIsActive && IsValid(Pool[i].Object))
		{
			// Call IPoolable interface before destroying
			if (Pool[i].Object->Implements<UPoolable>())
			{
				IPoolable::Execute_OnPoolDestroy(Pool[i].Object);
			}

			Factory->DestroyPoolObject(Pool[i].Object);
			ObjectToIndex.Remove(Pool[i].Object);
			Pool.RemoveAtSwap(i);
		}
	}

	// Rebuild object index map after swap removes
	ObjectToIndex.Empty();
	for (int32 i = 0; i < Pool.Num(); ++i)
	{
		ObjectToIndex.Add(Pool[i].Object, i);
	}

	bInactiveIndicesDirty = true;
	MarkStatsDirty();

	const int32 RemovedCount = InitialSize - Pool.Num();
	if (RemovedCount > 0)
	{
		UE_LOG(LogRavenPool, Log, TEXT("Cleared %d inactive objects from pool (Pool size: %d -> %d)"),
			RemovedCount, InitialSize, Pool.Num());
	}
}

void FRavenPool::Tick(float DeltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_Pool_Tick);

	// Handle periodic shrinking
	if (Policy.ShrinkInterval > 0.0f)
	{
		TimeSinceLastShrink += DeltaTime;
		if (TimeSinceLastShrink >= Policy.ShrinkInterval)
		{
			TimeSinceLastShrink = 0.0f;

			// Remove inactive objects down to minimum pool size
			const int32 InactiveCount = GetInactiveCount();
			const int32 TargetInactiveCount = FMath::Max(0, Policy.MinPoolSize - GetActiveCount());
			const int32 ToRemove = InactiveCount - TargetInactiveCount;

			if (ToRemove > 0)
			{
				UE_LOG(LogRavenPool, Log, TEXT("Shrinking pool for class %s: removing %d inactive objects"),
					*ObjectClass->GetName(), ToRemove);

				int32 Removed = 0;
				for (int32 i = Pool.Num() - 1; i >= 0 && Removed < ToRemove; --i)
				{
					if (!Pool[i].bIsActive && IsValid(Pool[i].Object))
					{
						// Call IPoolable interface
						if (Pool[i].Object->Implements<UPoolable>())
						{
							IPoolable::Execute_OnPoolDestroy(Pool[i].Object);
						}

						Factory->DestroyPoolObject(Pool[i].Object);
						ObjectToIndex.Remove(Pool[i].Object);
						Pool.RemoveAtSwap(i);
						Removed++;
					}
				}

				// Rebuild object index after removals
				ObjectToIndex.Empty();
				for (int32 i = 0; i < Pool.Num(); ++i)
				{
					ObjectToIndex.Add(Pool[i].Object, i);
				}

				bInactiveIndicesDirty = true;
				MarkStatsDirty();
			}
		}
	}

	// Handle max idle time
	if (Policy.MaxIdleTime > 0.0f)
	{
		const double CurrentTime = FPlatformTime::Seconds();
		int32 Removed = 0;

		for (int32 i = Pool.Num() - 1; i >= 0; --i)
		{
			if (!Pool[i].bIsActive && IsValid(Pool[i].Object))
			{
				const double IdleTime = CurrentTime - Pool[i].LastUsedTime;
				if (IdleTime >= Policy.MaxIdleTime)
				{
					// Don't go below minimum pool size
					if (Pool.Num() - Removed > Policy.MinPoolSize)
					{
						// Call IPoolable interface
						if (Pool[i].Object->Implements<UPoolable>())
						{
							IPoolable::Execute_OnPoolDestroy(Pool[i].Object);
						}

						Factory->DestroyPoolObject(Pool[i].Object);
						ObjectToIndex.Remove(Pool[i].Object);
						Pool.RemoveAtSwap(i);
						Removed++;
					}
				}
			}
		}

		if (Removed > 0)
		{
			// Rebuild object index after removals
			ObjectToIndex.Empty();
			for (int32 i = 0; i < Pool.Num(); ++i)
			{
				ObjectToIndex.Add(Pool[i].Object, i);
			}

			bInactiveIndicesDirty = true;
			MarkStatsDirty();

			UE_LOG(LogRavenPool, Log, TEXT("Removed %d idle objects from pool for class %s"),
				Removed, *ObjectClass->GetName());
		}
	}
}

int32 FRavenPool::ValidatePool()
{
	SCOPE_CYCLE_COUNTER(STAT_Pool_Validate);

	if (!Policy.bEnableValidation)
	{
		return 0;
	}

	int32 RemovedCount = 0;

	for (int32 i = Pool.Num() - 1; i >= 0; --i)
	{
		if (!Pool[i].Validate())
		{
			UE_LOG(LogRavenPool, Warning, TEXT("Removing invalid object from pool"));

			if (IsValid(Pool[i].Object))
			{
				// Call IPoolable interface
				if (Pool[i].Object->Implements<UPoolable>())
				{
					IPoolable::Execute_OnPoolDestroy(Pool[i].Object);
				}

				Factory->DestroyPoolObject(Pool[i].Object);
			}

			ObjectToIndex.Remove(Pool[i].Object);
			Pool.RemoveAtSwap(i);
			RemovedCount++;
		}
	}

	if (RemovedCount > 0)
	{
		// Rebuild object index
		ObjectToIndex.Empty();
		for (int32 i = 0; i < Pool.Num(); ++i)
		{
			ObjectToIndex.Add(Pool[i].Object, i);
		}

		bInactiveIndicesDirty = true;
		MarkStatsDirty();
	}

	return RemovedCount;
}

int32 FRavenPool::GetActiveCount() const
{
	if (bStatsDirty)
	{
		// Recalculate cached stats
		FRavenPool* MutableThis = const_cast<FRavenPool*>(this);
		MutableThis->CachedStats.ActiveCount = 0;
		MutableThis->CachedStats.InactiveCount = 0;

		for (const FRavenPoolEntry& Entry : Pool)
		{
			if (Entry.bIsActive)
			{
				MutableThis->CachedStats.ActiveCount++;
			}
			else
			{
				MutableThis->CachedStats.InactiveCount++;
			}
		}

		MutableThis->CachedStats.TotalCount = Pool.Num();
		MutableThis->CachedStats.CalculateUsagePercent();
		MutableThis->bStatsDirty = false;
	}

	return CachedStats.ActiveCount;
}

int32 FRavenPool::GetInactiveCount() const
{
	if (bStatsDirty)
	{
		GetActiveCount(); // This will recalculate all stats
	}
	return CachedStats.InactiveCount;
}

FRavenPoolStats FRavenPool::GetStats() const
{
	if (bStatsDirty)
	{
		GetActiveCount(); // This will recalculate all stats
	}
	return CachedStats;
}

void FRavenPool::SetPolicy(const FRavenPoolPolicy& InPolicy)
{
	Policy = InPolicy;

	// Recreate acquisition strategy if it changed
	if (!AcquisitionStrategy.IsValid() || Policy.AcquisitionStrategy != Policy.AcquisitionStrategy)
	{
		AcquisitionStrategy = FRavenPoolStrategyFactory::CreateStrategy(Policy.AcquisitionStrategy);
	}
}

int32 FRavenPool::FindInactiveObject()
{
	SCOPE_CYCLE_COUNTER(STAT_Pool_FindInactive);

	if (!AcquisitionStrategy.IsValid())
	{
		AcquisitionStrategy = FRavenPoolStrategyFactory::CreateStrategy(Policy.AcquisitionStrategy);
	}

	return AcquisitionStrategy->FindInactiveObject(Pool, InactiveIndices);
}

void FRavenPool::RebuildInactiveIndices()
{
	SCOPE_CYCLE_COUNTER(STAT_Pool_RebuildIndices);

	InactiveIndices.Reset();

	for (int32 i = 0; i < Pool.Num(); ++i)
	{
		if (!Pool[i].bIsActive)
		{
			InactiveIndices.Add(i);
		}
	}

	bInactiveIndicesDirty = false;
}
