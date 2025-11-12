// RavenStorm Copyright @ 2025-2025

#pragma once

#include "CoreMinimal.h"
#include "RavenPoolSubsystem.h"
#include "RavenPoolHandle.generated.h"

class URavenPoolSubsystem;

/**
 * Type-safe handle for pooled objects.
 * Automatically manages object lifetime and provides safe access to pooled objects.
 * When the handle is destroyed, the object is automatically released back to the pool.
 */
template <typename T>
struct TRavenPoolHandle
{
public:
	TRavenPoolHandle() = default;

	TRavenPoolHandle(T* InObject, UClass* InPoolClass, URavenPoolSubsystem* InSubsystem)
		: Object(InObject)
		  , PoolClass(InPoolClass)
		  , Subsystem(InSubsystem)
		  , bAutoRelease(true)
	{
	}

	// Move constructor
	TRavenPoolHandle(TRavenPoolHandle&& Other) noexcept
		: Object(MoveTemp(Other.Object))
		  , PoolClass(Other.PoolClass)
		  , Subsystem(Other.Subsystem)
		  , bAutoRelease(Other.bAutoRelease)
	{
		Other.bAutoRelease = false;
	}

	// Move assignment
	TRavenPoolHandle& operator=(TRavenPoolHandle&& Other) noexcept
	{
		if (this != &Other)
		{
			Release();
			Object = MoveTemp(Other.Object);
			PoolClass = Other.PoolClass;
			Subsystem = Other.Subsystem;
			bAutoRelease = Other.bAutoRelease;
			Other.bAutoRelease = false;
		}
		return *this;
	}

	// Delete copy constructor and assignment
	TRavenPoolHandle(const TRavenPoolHandle&) = delete;
	TRavenPoolHandle& operator=(const TRavenPoolHandle&) = delete;

	~TRavenPoolHandle()
	{
		if (bAutoRelease)
		{
			Release();
		}
	}

	/**
	 * Gets the pooled object.
	 * @return Pointer to the object, or nullptr if invalid
	 */
	T* Get() const
	{
		return Object.Get();
	}

	/**
	 * Gets the pooled object (arrow operator).
	 * @return Pointer to the object
	 */
	T* operator->() const
	{
		return Object.Get();
	}

	/**
	 * Gets the pooled object (dereference operator).
	 * @return Reference to the object
	 */
	T& operator*() const
	{
		return *Object.Get();
	}

	/**
	 * Checks if the handle is valid.
	 * @return True if the object is valid, false otherwise
	 */
	bool IsValid() const
	{
		return Object.IsValid();
	}

	/**
	 * Implicit conversion to bool for validity checks.
	 */
	explicit operator bool() const
	{
		return IsValid();
	}

	/**
	 * Releases the object back to the pool.
	 * After calling this, the handle will no longer be valid.
	 */
	void Release()
	{
		if (Object.IsValid() && Subsystem.IsValid())
		{
			Subsystem->Release(Object.Get());
		}
		bAutoRelease = false;
	}

	/**
	 * Detaches the object from the handle without releasing it back to the pool.
	 * The caller becomes responsible for managing the object.
	 * @return The detached object pointer
	 */
	T* Detach()
	{
		bAutoRelease = false;
		return Object.Get();
	}

	/**
	 * Resets the handle, releasing the object back to the pool if auto-release is enabled.
	 */
	void Reset()
	{
		Release();
		Object.Reset();
		PoolClass = nullptr;
		Subsystem = nullptr;
	}

	/**
	 * Gets the pool class this object belongs to.
	 * @return The pool class
	 */
	UClass* GetPoolClass() const
	{
		return PoolClass;
	}

private:
	TWeakObjectPtr<T> Object;
	TObjectPtr<UClass> PoolClass = nullptr;
	TWeakObjectPtr<URavenPoolSubsystem> Subsystem;
	bool bAutoRelease = false;
};

/**
 * Non-templated base handle for Blueprint support.
 * Note: Structs cannot expose functions to Blueprint, so this is primarily for C++ use.
 * For Blueprint support, use the subsystem directly.
 */
USTRUCT(BlueprintType)
struct RAVEN_API FRavenPoolHandle
{
	GENERATED_BODY()

public:
	FRavenPoolHandle() = default;

	FRavenPoolHandle(UObject* InObject, UClass* InPoolClass, URavenPoolSubsystem* InSubsystem)
		: Object(InObject)
		  , PoolClass(InPoolClass)
		  , Subsystem(InSubsystem)
		  , bAutoRelease(true)
	{
	}

	~FRavenPoolHandle()
	{
		if (bAutoRelease)
		{
			Release();
		}
	}

	/**
	 * Gets the pooled object.
	 * @return Pointer to the object, or nullptr if invalid
	 */
	UObject* Get() const
	{
		return Object.Get();
	}

	/**
	 * Checks if the handle is valid.
	 * @return True if the object is valid, false otherwise
	 */
	bool IsValid() const
	{
		return Object.IsValid();
	}

	/**
	 * Releases the object back to the pool.
	 */
	void Release();

	/**
	 * Detaches the object from the handle without releasing it.
	 * @return The detached object
	 */
	UObject* Detach()
	{
		bAutoRelease = false;
		return Object.Get();
	}

	/**
	 * Resets the handle.
	 */
	void Reset()
	{
		Release();
		Object.Reset();
		PoolClass = nullptr;
		Subsystem = nullptr;
	}

	/**
	 * Gets the pool class this object belongs to.
	 * @return The pool class
	 */
	UClass* GetPoolClass() const
	{
		return PoolClass;
	}

private:
	UPROPERTY()
	TWeakObjectPtr<UObject> Object;

	UPROPERTY()
	TObjectPtr<UClass> PoolClass = nullptr;

	UPROPERTY()
	TWeakObjectPtr<URavenPoolSubsystem> Subsystem;

	bool bAutoRelease = false;
};
