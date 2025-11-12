// RavenStorm Copyright @ 2025-2025

#include "Pool/RavenPool.h"

#include "Pool/Factory/RavenPoolFactoryUObject.h"

DEFINE_LOG_CATEGORY_STATIC(LogRavenPool, Log, All);

FRavenPool::FRavenPool(UClass* InObjectClass)
	: ObjectClass(InObjectClass)
{
}

UObject* FRavenPool::Acquire()
{
	if (!IsValid(Factory) || !IsValid(ObjectClass))
	{
		UE_LOG(LogRavenPool, Error, TEXT("Cannot acquire object: Factory or ObjectClass is invalid"));
		return nullptr;
	}

	// Try to reuse an inactive object from the pool
	for (FRavenPoolEntry& Entry : Pool)
	{
		if (!Entry.bIsActive && IsValid(Entry.Object))
		{
			Entry.bIsActive = true;
			Factory->PrepareForUsage(Entry.Object);
			UE_LOG(LogRavenPool, Verbose, TEXT("Reusing pooled object of class %s"), *ObjectClass->GetName());
			return Entry.Object;
		}
	}

	// Check if we've reached the maximum pool size
	if (MaxPoolSize > 0 && Pool.Num() >= MaxPoolSize)
	{
		UE_LOG(LogRavenPool, Warning, TEXT("Cannot create new object: Pool for class %s has reached max size %d"),
			*ObjectClass->GetName(), MaxPoolSize);
		return nullptr;
	}

	// No inactive object found, create a new one
	UObject* Object = Factory->CreatePoolObject(ObjectClass);
	if (!IsValid(Object))
	{
		UE_LOG(LogRavenPool, Error, TEXT("Failed to create new pooled object of class %s"), *ObjectClass->GetName());
		return nullptr;
	}

	Factory->PrepareForUsage(Object);
	Pool.Emplace(FRavenPoolEntry{.bIsActive = true, .Object = Object});
	UE_LOG(LogRavenPool, Log, TEXT("Created new pooled object of class %s (Pool size: %d)"), *ObjectClass->GetName(), Pool.Num());
	return Object;
}

bool FRavenPool::Release(UObject* Object)
{
	if (!IsValid(Object) || !IsValid(Factory))
	{
		UE_LOG(LogRavenPool, Warning, TEXT("Cannot release object: Object or Factory is invalid"));
		return false;
	}

	if (FRavenPoolEntry* Entry = Pool.FindByKey(Object))
	{
		if (Entry->bIsActive)
		{
			Entry->bIsActive = false;
			Factory->PrepareForStorage(Entry->Object);
			UE_LOG(LogRavenPool, Verbose, TEXT("Released object of class %s back to pool"), *ObjectClass->GetName());
			return true;
		}
		else
		{
			UE_LOG(LogRavenPool, Warning, TEXT("Attempted to release already inactive object of class %s"), *ObjectClass->GetName());
			return false;
		}
	}

	UE_LOG(LogRavenPool, Warning, TEXT("Attempted to release object that doesn't belong to this pool"));
	return false;
}

int32 FRavenPool::GetActiveCount() const
{
	int32 Count = 0;
	for (const FRavenPoolEntry& Entry : Pool)
	{
		if (Entry.bIsActive)
		{
			++Count;
		}
	}
	return Count;
}

int32 FRavenPool::GetInactiveCount() const
{
	int32 Count = 0;
	for (const FRavenPoolEntry& Entry : Pool)
	{
		if (!Entry.bIsActive)
		{
			++Count;
		}
	}
	return Count;
}

void FRavenPool::PreWarm(int32 Count)
{
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

	for (int32 i = 0; i < ObjectsToCreate; ++i)
	{
		UObject* Object = Factory->CreatePoolObject(ObjectClass);
		if (IsValid(Object))
		{
			Factory->PrepareForStorage(Object);
			Pool.Emplace(FRavenPoolEntry{.bIsActive = false, .Object = Object});
		}
		else
		{
			UE_LOG(LogRavenPool, Error, TEXT("Failed to create object %d/%d during pre-warming"),
				i + 1, ObjectsToCreate);
		}
	}

	UE_LOG(LogRavenPool, Log, TEXT("Pre-warmed pool for class %s (Pool size: %d)"),
		*ObjectClass->GetName(), Pool.Num());
}

void FRavenPool::ClearInactive()
{
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
			Factory->DestroyPoolObject(Pool[i].Object);
			Pool.RemoveAtSwap(i);
		}
	}

	const int32 RemovedCount = InitialSize - Pool.Num();
	if (RemovedCount > 0)
	{
		UE_LOG(LogRavenPool, Log, TEXT("Cleared %d inactive objects from pool (Pool size: %d -> %d)"),
			RemovedCount, InitialSize, Pool.Num());
	}
}
