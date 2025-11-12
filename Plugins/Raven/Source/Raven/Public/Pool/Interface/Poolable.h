// RavenStorm Copyright @ 2025-2025

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Poolable.generated.h"

/**
 * Interface for objects that can be pooled.
 * Implement this interface to receive lifecycle callbacks when objects are acquired/released from the pool.
 */
UINTERFACE(MinimalAPI, Blueprintable)
class UPoolable : public UInterface
{
	GENERATED_BODY()
};

class RAVEN_API IPoolable
{
	GENERATED_BODY()

public:
	/**
	 * Called when the object is acquired from the pool.
	 * Use this to reset state and prepare the object for use.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Raven|Pool")
	void OnAcquiredFromPool();
	virtual void OnAcquiredFromPool_Implementation() {}

	/**
	 * Called when the object is returned to the pool.
	 * Use this to clean up state and prepare the object for storage.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Raven|Pool")
	void OnReturnedToPool();
	virtual void OnReturnedToPool_Implementation() {}

	/**
	 * Checks if the object is valid for reuse.
	 * If this returns false, the pool will destroy the object instead of reusing it.
	 * @return True if the object can be reused, false otherwise
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Raven|Pool")
	bool IsValidForReuse() const;
	virtual bool IsValidForReuse_Implementation() const { return true; }

	/**
	 * Called when the object is about to be destroyed by the pool.
	 * Use this for final cleanup before destruction.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Raven|Pool")
	void OnPoolDestroy();
	virtual void OnPoolDestroy_Implementation() {}
};
