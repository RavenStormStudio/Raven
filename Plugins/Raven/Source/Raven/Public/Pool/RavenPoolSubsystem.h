// RavenStorm Copyright @ 2025-2025

#pragma once

#include "CoreMinimal.h"
#include "RavenPool.h"

#include "Subsystems/WorldSubsystem.h"
#include "RavenPoolSubsystem.generated.h"

/**
 * World subsystem that manages object pools.
 * Provides centralized access to acquire and release pooled objects.
 */
UCLASS()
class RAVEN_API URavenPoolSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	/**
	 * Acquires an object of the specified class from the pool.
	 * @param Class The class of object to acquire
	 * @return The acquired object, or nullptr if acquisition fails
	 */
	UFUNCTION(BlueprintCallable, Category = "Raven|Pool")
	UObject* Acquire(UClass* Class);

	/**
	 * Releases an object back to its pool for reuse.
	 * @param Object The object to release
	 * @return True if the object was successfully released, false otherwise
	 */
	UFUNCTION(BlueprintCallable, Category = "Raven|Pool")
	bool Release(UObject* Object);

	/**
	 * Registers a factory for creating objects of a specific class.
	 * @param Class The class to register the factory for
	 * @param FactoryClass The factory class to use for creating objects
	 */
	void AddFactory(UClass* Class, TSubclassOf<URavenPoolFactoryUObject> FactoryClass);

	/**
	 * Removes a registered factory for a specific class.
	 * @param Class The class to remove the factory for
	 */
	void RemoveFactory(UClass* Class);

	/**
	 * Gets the total number of objects in the pool for a specific class.
	 * @param ObjectClass The class to check
	 * @return The pool size, or 0 if no pool exists
	 */
	UFUNCTION(BlueprintPure, Category = "Raven|Pool")
	int32 GetPoolSize(UClass* ObjectClass) const;

	/**
	 * Gets the number of active objects in the pool for a specific class.
	 * @param ObjectClass The class to check
	 * @return The number of active objects, or 0 if no pool exists
	 */
	UFUNCTION(BlueprintPure, Category = "Raven|Pool")
	int32 GetActiveCount(UClass* ObjectClass) const;

	/**
	 * Gets the number of inactive objects in the pool for a specific class.
	 * @param ObjectClass The class to check
	 * @return The number of inactive objects, or 0 if no pool exists
	 */
	UFUNCTION(BlueprintPure, Category = "Raven|Pool")
	int32 GetInactiveCount(UClass* ObjectClass) const;

	/**
	 * Clears all inactive objects from the pool for a specific class.
	 * @param ObjectClass The class to clear inactive objects for
	 */
	UFUNCTION(BlueprintCallable, Category = "Raven|Pool")
	void ClearInactiveObjects(UClass* ObjectClass);

	/**
	 * Gets statistics for all active pools.
	 * Useful for debugging and monitoring pool usage.
	 */
	UFUNCTION(BlueprintCallable, Category = "Raven|Pool")
	void LogPoolStatistics() const;

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool DoesSupportWorldType(EWorldType::Type WorldType) const override;

	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override { return true; }
	virtual bool IsTickableInEditor() const override { return false; }
	virtual bool IsTickableWhenPaused() const override { return false; }

protected:
	/** Gets or creates a pool for the specified class */
	virtual FRavenPool* GetPool(UClass* ObjectClass);

	/** Gets the pool for a specific class (for statistics/debugging) */
	const FRavenPool* GetPoolForClass(UClass* ObjectClass) const;

private:
	/** All active pools */
	UPROPERTY()
	TArray<FRavenPool> Pools;

	/** Registered factories for creating pooled objects */
	UPROPERTY()
	TMap<TObjectPtr<UClass>, TObjectPtr<URavenPoolFactoryUObject>> Factories;
};
