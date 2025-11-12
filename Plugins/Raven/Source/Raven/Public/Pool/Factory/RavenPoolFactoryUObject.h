// RavenStorm Copyright @ 2025-2025

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RavenPoolFactoryUObject.generated.h"

/**
 * Base factory class for creating and managing pooled objects.
 * Override this class to customize how objects are created, destroyed, and prepared for use/storage.
 */
UCLASS(BlueprintType, Blueprintable)
class RAVEN_API URavenPoolFactoryUObject : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Creates a new pooled object instance.
	 * @param Class The class of object to create
	 * @return The created object
	 */
	virtual UObject* CreatePoolObject(UClass* Class);

	/**
	 * Destroys a pooled object.
	 * @param Object The object to destroy
	 */
	virtual void DestroyPoolObject(UObject* Object);

	/**
	 * Prepares an object for storage in the pool (when released).
	 * Use this to reset state, disable components, etc.
	 * @param Object The object being stored
	 */
	virtual void PrepareForStorage(UObject* Object);

	/**
	 * Prepares an object for usage (when acquired from pool).
	 * Use this to restore state, enable components, etc.
	 * @param Object The object being acquired
	 */
	virtual void PrepareForUsage(UObject* Object);
};
