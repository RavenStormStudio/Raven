// RavenStorm Copyright @ 2025-2025

#include "Pool/Strategy/RavenPoolStrategy.h"
#include "Pool/RavenPool.h"

int32 FRavenPoolFIFOStrategy::FindInactiveObject(const TArray<FRavenPoolEntry>& Pool, const TArray<int32>& InactiveIndices)
{
	// Return the first inactive object (oldest)
	return InactiveIndices.Num() > 0 ? InactiveIndices[0] : INDEX_NONE;
}

int32 FRavenPoolLIFOStrategy::FindInactiveObject(const TArray<FRavenPoolEntry>& Pool, const TArray<int32>& InactiveIndices)
{
	// Return the last inactive object (most recently released)
	return InactiveIndices.Num() > 0 ? InactiveIndices.Last() : INDEX_NONE;
}

int32 FRavenPoolLRUStrategy::FindInactiveObject(const TArray<FRavenPoolEntry>& Pool, const TArray<int32>& InactiveIndices)
{
	if (InactiveIndices.Num() == 0)
	{
		return INDEX_NONE;
	}

	// Find the inactive object with the oldest last used time
	int32 OldestIndex = InactiveIndices[0];
	double OldestTime = Pool[OldestIndex].LastUsedTime;

	for (int32 i = 1; i < InactiveIndices.Num(); ++i)
	{
		const int32 Index = InactiveIndices[i];
		if (Pool[Index].LastUsedTime < OldestTime)
		{
			OldestTime = Pool[Index].LastUsedTime;
			OldestIndex = Index;
		}
	}

	return OldestIndex;
}

int32 FRavenPoolRandomStrategy::FindInactiveObject(const TArray<FRavenPoolEntry>& Pool, const TArray<int32>& InactiveIndices)
{
	if (InactiveIndices.Num() == 0)
	{
		return INDEX_NONE;
	}

	// Select a random inactive object
	const int32 RandomIndex = FMath::RandRange(0, InactiveIndices.Num() - 1);
	return InactiveIndices[RandomIndex];
}

TSharedPtr<IRavenPoolAcquisitionStrategy> FRavenPoolStrategyFactory::CreateStrategy(ERavenPoolAcquisitionStrategy StrategyType)
{
	switch (StrategyType)
	{
	case ERavenPoolAcquisitionStrategy::FIFO:
		return MakeShared<FRavenPoolFIFOStrategy>();

	case ERavenPoolAcquisitionStrategy::LIFO:
		return MakeShared<FRavenPoolLIFOStrategy>();

	case ERavenPoolAcquisitionStrategy::LRU:
		return MakeShared<FRavenPoolLRUStrategy>();

	case ERavenPoolAcquisitionStrategy::Random:
		return MakeShared<FRavenPoolRandomStrategy>();

	default:
		return MakeShared<FRavenPoolFIFOStrategy>();
	}
}
