// RavenStorm Copyright @ 2025-2025

#pragma once

#include "CoreMinimal.h"
#include "RavenPool.generated.h"

class URavenPoolFactoryUObject;

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
	 * Clears all inactive objects from the pool.
	 * Active objects will remain until released.
	 */
	void ClearInactive();

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

	friend class RAVEN_API URavenPoolSubsystem;

public:
	friend RAVEN_API bool operator==(const FRavenPool& A, const UClass* B) { return A.ObjectClass == B; }
};
