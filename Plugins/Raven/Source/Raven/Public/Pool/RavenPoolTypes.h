// RavenStorm Copyright @ 2025-2025

#pragma once

#include "CoreMinimal.h"
#include "RavenPoolTypes.generated.h"

class URavenPoolSubsystem;

/**
 * Acquisition strategy determines how objects are selected from the pool.
 */
UENUM(BlueprintType)
enum class ERavenPoolAcquisitionStrategy : uint8
{
	/** First-In-First-Out - reuses oldest inactive object */
	FIFO UMETA(DisplayName = "FIFO (First In First Out)"),

	/** Last-In-First-Out - reuses most recently released object */
	LIFO UMETA(DisplayName = "LIFO (Last In First Out)"),

	/** Least Recently Used - reuses object that hasn't been used the longest */
	LRU UMETA(DisplayName = "LRU (Least Recently Used)"),

	/** Random - selects a random inactive object */
	Random UMETA(DisplayName = "Random")
};

/**
 * Pool policy configuration for advanced pool management.
 */
USTRUCT(BlueprintType)
struct RAVEN_API FRavenPoolPolicy
{
	GENERATED_BODY()

	/** Maximum time an object can remain idle before being destroyed (0 = never destroy) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Policy", meta = (ClampMin = "0"))
	float MaxIdleTime = 0.0f;

	/** Interval for periodic pool shrinking (0 = disabled) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Policy", meta = (ClampMin = "0"))
	float ShrinkInterval = 0.0f;

	/** Minimum number of objects to keep in pool during shrinking */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Policy", meta = (ClampMin = "0"))
	int32 MinPoolSize = 0;

	/** Enable validation checks on pooled objects */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Policy")
	bool bEnableValidation = true;

	/** Acquisition strategy for selecting objects from the pool */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Policy")
	ERavenPoolAcquisitionStrategy AcquisitionStrategy = ERavenPoolAcquisitionStrategy::FIFO;
};

/**
 * Context passed to factory when creating objects.
 */
USTRUCT(BlueprintType)
struct RAVEN_API FPoolCreationContext
{
	GENERATED_BODY()

	/** The pool requesting the object creation */
	UPROPERTY()
	TObjectPtr<URavenPoolSubsystem> PoolSubsystem = nullptr;

	/** The class of object to create */
	UPROPERTY()
	TObjectPtr<UClass> ObjectClass = nullptr;

	/** Whether this is for pre-warming */
	UPROPERTY()
	bool bIsPreWarming = false;

	/** Current pool size */
	UPROPERTY()
	int32 CurrentPoolSize = 0;
};

/**
 * Context passed to factory when resetting objects.
 */
USTRUCT(BlueprintType)
struct RAVEN_API FPoolResetContext
{
	GENERATED_BODY()

	/** Whether the object is being prepared for storage (true) or usage (false) */
	UPROPERTY()
	bool bIsStorage = false;

	/** The pool this object belongs to */
	UPROPERTY()
	TObjectPtr<URavenPoolSubsystem> PoolSubsystem = nullptr;
};

/**
 * Statistics for a pool.
 */
USTRUCT(BlueprintType)
struct RAVEN_API FRavenPoolStats
{
	GENERATED_BODY()

	/** Total number of objects in pool */
	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	int32 TotalCount = 0;

	/** Number of active (in-use) objects */
	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	int32 ActiveCount = 0;

	/** Number of inactive (available) objects */
	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	int32 InactiveCount = 0;

	/** Total number of objects created over lifetime */
	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	int32 TotalCreated = 0;

	/** Total number of acquisitions */
	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	int32 TotalAcquisitions = 0;

	/** Total number of releases */
	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	int32 TotalReleases = 0;

	/** Number of objects reused from pool */
	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	int32 TotalReuses = 0;

	/** Peak pool size */
	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	int32 PeakPoolSize = 0;

	/** Usage percentage (active / total) */
	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	float UsagePercent = 0.0f;

	void CalculateUsagePercent()
	{
		UsagePercent = TotalCount > 0 ? (static_cast<float>(ActiveCount) / TotalCount) * 100.0f : 0.0f;
	}
};

/**
 * Event data for pool events.
 */
USTRUCT(BlueprintType)
struct RAVEN_API FPoolEventData
{
	GENERATED_BODY()

	/** The object involved in the event */
	UPROPERTY(BlueprintReadOnly, Category = "Event")
	TObjectPtr<UObject> Object = nullptr;

	/** The class of the pool */
	UPROPERTY(BlueprintReadOnly, Category = "Event")
	TObjectPtr<UClass> PoolClass = nullptr;

	/** Current pool statistics */
	UPROPERTY(BlueprintReadOnly, Category = "Event")
	FRavenPoolStats Stats;

	/** Event timestamp */
	UPROPERTY(BlueprintReadOnly, Category = "Event")
	double Timestamp = 0.0;
};
