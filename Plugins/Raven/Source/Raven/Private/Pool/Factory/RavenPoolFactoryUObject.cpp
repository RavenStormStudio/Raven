// RavenStorm Copyright @ 2025-2025

#include "Pool/Factory/RavenPoolFactoryUObject.h"

UObject* URavenPoolFactoryUObject::CreatePoolObject(UClass* Class)
{
	return NewObject<UObject>(GetOuter(), Class);
}

void URavenPoolFactoryUObject::DestroyPoolObject(UObject* Object)
{
	Object->ConditionalBeginDestroy();
}

void URavenPoolFactoryUObject::PrepareForStorage(UObject* Object)
{
}

void URavenPoolFactoryUObject::PrepareForUsage(UObject* Object)
{
}
