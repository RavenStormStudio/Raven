// RavenStorm Copyright @ 2025-2025

#pragma once

#include "CoreMinimal.h"
#include "RavenPoolFactoryUObject.h"
#include "RavenPoolActorFactory.generated.h"

/**
 * Factory for creating and managing pooled actors.
 * Handles actor-specific operations like spawning, destruction, and state management.
 */
UCLASS()
class RAVEN_API URavenPoolActorFactory : public URavenPoolFactoryUObject
{
	GENERATED_BODY()

public:
	virtual UObject* CreatePoolObject(UClass* Class) override;
	virtual void DestroyPoolObject(UObject* Object) override;

	/**
	 * Prepares an actor for storage by moving it out of view and disabling it.
	 * Disables tick, collision, and visibility.
	 */
	virtual void PrepareForStorage(UObject* Object) override;

	/**
	 * Prepares an actor for usage by enabling it.
	 * Enables tick, collision, and visibility.
	 * Note: Does not reset location - caller should set desired position.
	 */
	virtual void PrepareForUsage(UObject* Object) override;
};
