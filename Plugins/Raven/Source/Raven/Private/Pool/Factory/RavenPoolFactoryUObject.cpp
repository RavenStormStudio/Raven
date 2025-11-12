// RavenStorm Copyright @ 2025-2025

#include "Pool/Factory/RavenPoolFactoryUObject.h"
#include "Pool/RavenPoolTypes.h"
#include "Pool/RavenPoolStats.h"

UObject* URavenPoolFactoryUObject::CreatePoolObject_Implementation(UClass* Class)
{
	SCOPE_CYCLE_COUNTER(STAT_Factory_Create);
	return NewObject<UObject>(GetOuter(), Class);
}

UObject* URavenPoolFactoryUObject::CreatePoolObjectWithContext_Implementation(const FPoolCreationContext& Context)
{
	return CreatePoolObject(Context.ObjectClass);
}

void URavenPoolFactoryUObject::DestroyPoolObject_Implementation(UObject* Object)
{
	SCOPE_CYCLE_COUNTER(STAT_Factory_Destroy);
	if (IsValid(Object))
	{
		Object->ConditionalBeginDestroy();
	}
}

void URavenPoolFactoryUObject::PrepareForStorage_Implementation(UObject* Object)
{
	SCOPE_CYCLE_COUNTER(STAT_Factory_PrepareStorage);
	// Default implementation does nothing
}

void URavenPoolFactoryUObject::PrepareForStorageWithContext_Implementation(UObject* Object, const FPoolResetContext& Context)
{
	SCOPE_CYCLE_COUNTER(STAT_Factory_PrepareStorage);
	PrepareForStorage(Object);
}

void URavenPoolFactoryUObject::PrepareForUsage_Implementation(UObject* Object)
{
	SCOPE_CYCLE_COUNTER(STAT_Factory_PrepareUsage);
	// Default implementation does nothing
}

void URavenPoolFactoryUObject::PrepareForUsageWithContext_Implementation(UObject* Object, const FPoolResetContext& Context)
{
	SCOPE_CYCLE_COUNTER(STAT_Factory_PrepareUsage);
	PrepareForUsage(Object);
}

bool URavenPoolFactoryUObject::CanCreateClass_Implementation(UClass* Class) const
{
	return IsValid(Class);
}
