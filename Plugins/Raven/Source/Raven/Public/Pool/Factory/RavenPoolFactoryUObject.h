// RavenStorm Copyright @ 2025-2025

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RavenPoolFactoryUObject.generated.h"

/**
 * Base factory class for creating and managing pooled objects.
 * Override this class to customize how objects are created, destroyed, and prepared for use/storage.
 */
UCLASS(BlueprintType, Blueprintable, Abstract)
class RAVEN_API URavenPoolFactoryUObject : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Creates a new pooled object instance.
	 * @param Class The class of object to create
	 * @return The created object
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Raven|Pool")
	UObject* CreatePoolObject(UClass* Class);
	virtual UObject* CreatePoolObject_Implementation(UClass* Class);

	/**
	 * Creates a new pooled object with context.
	 * Override this for more control over object creation.
	 * @param Context Creation context with additional information
	 * @return The created object
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Raven|Pool")
	UObject* CreatePoolObjectWithContext(const FPoolCreationContext& Context);
	virtual UObject* CreatePoolObjectWithContext_Implementation(const FPoolCreationContext& Context);

	/**
	 * Destroys a pooled object.
	 * @param Object The object to destroy
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Raven|Pool")
	void DestroyPoolObject(UObject* Object);
	virtual void DestroyPoolObject_Implementation(UObject* Object);

	/**
	 * Prepares an object for storage in the pool (when released).
	 * Use this to reset state, disable components, etc.
	 * @param Object The object being stored
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Raven|Pool")
	void PrepareForStorage(UObject* Object);
	virtual void PrepareForStorage_Implementation(UObject* Object);

	/**
	 * Prepares an object for storage with context.
	 * @param Object The object being stored
	 * @param Context Reset context with additional information
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Raven|Pool")
	void PrepareForStorageWithContext(UObject* Object, const FPoolResetContext& Context);
	virtual void PrepareForStorageWithContext_Implementation(UObject* Object, const FPoolResetContext& Context);

	/**
	 * Prepares an object for usage (when acquired from pool).
	 * Use this to restore state, enable components, etc.
	 * @param Object The object being acquired
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Raven|Pool")
	void PrepareForUsage(UObject* Object);
	virtual void PrepareForUsage_Implementation(UObject* Object);

	/**
	 * Prepares an object for usage with context.
	 * @param Object The object being acquired
	 * @param Context Reset context with additional information
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Raven|Pool")
	void PrepareForUsageWithContext(UObject* Object, const FPoolResetContext& Context);
	virtual void PrepareForUsageWithContext_Implementation(UObject* Object, const FPoolResetContext& Context);

	/**
	 * Validates that this factory can create objects of the specified class.
	 * @param Class The class to validate
	 * @return True if this factory can create objects of this class
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Raven|Pool")
	bool CanCreateClass(UClass* Class) const;
	virtual bool CanCreateClass_Implementation(UClass* Class) const;
};
