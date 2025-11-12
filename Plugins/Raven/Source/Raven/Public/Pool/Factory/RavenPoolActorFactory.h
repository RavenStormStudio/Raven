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
	virtual UObject* CreatePoolObject_Implementation(UClass* Class) override;
	virtual UObject* CreatePoolObjectWithContext_Implementation(const FPoolCreationContext& Context) override;
	virtual void DestroyPoolObject_Implementation(UObject* Object) override;

	/**
	 * Prepares an actor for storage by moving it out of view and disabling it.
	 * Disables tick, collision, and visibility.
	 */
	virtual void PrepareForStorage_Implementation(UObject* Object) override;

	/**
	 * Prepares an actor for usage by enabling it.
	 * Enables tick, collision, and visibility.
	 * Note: Does not reset location - caller should set desired position.
	 */
	virtual void PrepareForUsage_Implementation(UObject* Object) override;

	virtual bool CanCreateClass_Implementation(UClass* Class) const override;

protected:
	/** Location to move actors to when stored in the pool */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pool")
	FVector StorageLocation = FVector(0.0f, 0.0f, -10000.0f);

	/** Whether to disable components when storing actors */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pool")
	bool bDisableComponents = true;
};
