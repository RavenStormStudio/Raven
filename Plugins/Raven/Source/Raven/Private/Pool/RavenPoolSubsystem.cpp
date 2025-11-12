// RavenStorm Copyright @ 2025-2025

#include "Pool/RavenPoolSubsystem.h"

#include "Pool/RavenPoolDeveloperSettings.h"
#include "Pool/Factory/RavenPoolFactoryUObject.h"
#include "Pool/RavenPoolStats.h"

DEFINE_LOG_CATEGORY_STATIC(LogRavenPoolSubsystem, Log, All);

UObject* URavenPoolSubsystem::Acquire(UClass* Class)
{
	SCOPE_CYCLE_COUNTER(STAT_PoolSubsystem_Acquire);

	if (!IsValid(Class))
	{
		UE_LOG(LogRavenPoolSubsystem, Error, TEXT("Cannot acquire object: Class is invalid"));
		return nullptr;
	}

	FRavenPool* Pool = GetPool(Class);
	if (!Pool)
	{
		UE_LOG(LogRavenPoolSubsystem, Error, TEXT("No pool found for class %s. Make sure a factory is registered for this class."), *Class->GetName());
		return nullptr;
	}
	return Pool->Acquire();
}

bool URavenPoolSubsystem::Release(UObject* Object)
{
	SCOPE_CYCLE_COUNTER(STAT_PoolSubsystem_Release);

	if (!IsValid(Object))
	{
		return false;
	}

	FRavenPool* Pool = GetPool(Object->GetClass());
	if (!Pool)
	{
		return false;
	}
	return Pool->Release(Object);
}

void URavenPoolSubsystem::AddFactory(UClass* Class, const TSubclassOf<URavenPoolFactoryUObject> FactoryClass)
{
	UObject* FactoryUObject = NewObject<UObject>(this, FactoryClass.Get());
	URavenPoolFactoryUObject* Factory = Cast<URavenPoolFactoryUObject>(FactoryUObject);
	ensureAlways(Factory != nullptr);
	Factories.Emplace(Class, Factory);
}

void URavenPoolSubsystem::RemoveFactory(UClass* Class)
{
	if (const TObjectPtr<URavenPoolFactoryUObject>* FoundFactory = Factories.Find(Class))
	{
		if (IsValid(*FoundFactory))
		{
			(*FoundFactory)->ConditionalBeginDestroy();
		}
		Factories.Remove(Class);
	}
}

void URavenPoolSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	SCOPE_CYCLE_COUNTER(STAT_PoolSubsystem_Initialize);

	Super::Initialize(Collection);

	UE_LOG(LogRavenPoolSubsystem, Log, TEXT("Initializing RavenPoolSubsystem"));

	const URavenPoolDeveloperSettings* PoolSettings = GetDefault<URavenPoolDeveloperSettings>();
	for (const FRavenPoolConfig& PoolConfig : PoolSettings->GetPoolConfigs())
	{
		if (PoolConfig.Class && !PoolConfig.Factory.IsNull())
		{
			if (const TSubclassOf<URavenPoolFactoryUObject> FactoryClass = PoolConfig.Factory.LoadSynchronous())
			{
				AddFactory(PoolConfig.Class, FactoryClass);
				UE_LOG(LogRavenPoolSubsystem, Log, TEXT("Registered factory %s for class %s"),
				       *FactoryClass->GetName(), *PoolConfig.Class->GetName());

				// Get or create the pool and configure it
				if (FRavenPool* Pool = GetPool(PoolConfig.Class))
				{
					Pool->SetMaxPoolSize(PoolConfig.MaxPoolSize);
					Pool->SetPolicy(PoolConfig.Policy);

					// Pre-warm the pool if configured
					if (PoolConfig.InitialPoolSize > 0)
					{
						Pool->PreWarm(PoolConfig.InitialPoolSize);
					}
				}
			}
			else
			{
				UE_LOG(LogRavenPoolSubsystem, Error, TEXT("Failed to load factory for class %s"),
				       *PoolConfig.Class->GetName());
			}
		}
	}
}

void URavenPoolSubsystem::Deinitialize()
{
	UE_LOG(LogRavenPoolSubsystem, Log, TEXT("Deinitializing RavenPoolSubsystem"));

	for (TTuple<TObjectPtr<UClass>, TObjectPtr<URavenPoolFactoryUObject>>& Iterator : Factories)
	{
		if (IsValid(Iterator.Value))
		{
			Iterator.Value->ConditionalBeginDestroy();
		}
	}
	Factories.Empty();
	Pools.Empty();

	Super::Deinitialize();
}

bool URavenPoolSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

FRavenPool* URavenPoolSubsystem::GetPool(UClass* ObjectClass)
{
	SCOPE_CYCLE_COUNTER(STAT_PoolSubsystem_GetPool);

	if (!IsValid(ObjectClass))
	{
		UE_LOG(LogRavenPoolSubsystem, Error, TEXT("GetPool called with invalid ObjectClass"));
		return nullptr;
	}

	FRavenPool* Pool = Pools.FindByKey(ObjectClass);
	if (!Pool)
	{
		const TObjectPtr<URavenPoolFactoryUObject>* FoundFactory = Factories.Find(ObjectClass);
		if (!FoundFactory || !IsValid(*FoundFactory))
		{
			UE_LOG(LogRavenPoolSubsystem, Error, TEXT("No factory registered for class %s"), *ObjectClass->GetName());
			return nullptr;
		}

		Pool = &Pools.AddDefaulted_GetRef();
		Pool->ObjectClass = ObjectClass;
		Pool->Factory = *FoundFactory;
		UE_LOG(LogRavenPoolSubsystem, Log, TEXT("Created new pool for class %s"), *ObjectClass->GetName());
	}
	return Pool;
}

const FRavenPool* URavenPoolSubsystem::GetPoolForClass(UClass* ObjectClass) const
{
	if (!IsValid(ObjectClass))
	{
		return nullptr;
	}
	return Pools.FindByKey(ObjectClass);
}

void URavenPoolSubsystem::Tick(float DeltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_PoolSubsystem_Tick);

	Super::Tick(DeltaTime);

	// Tick all pools for maintenance (idle cleanup, periodic shrinking, etc.)
	for (FRavenPool& Pool : Pools)
	{
		Pool.Tick(DeltaTime);
	}
}

TStatId URavenPoolSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(URavenPoolSubsystem, STATGROUP_Tickables);
}

int32 URavenPoolSubsystem::GetPoolSize(UClass* ObjectClass) const
{
	const FRavenPool* Pool = GetPoolForClass(ObjectClass);
	return Pool ? Pool->GetPoolSize() : 0;
}

int32 URavenPoolSubsystem::GetActiveCount(UClass* ObjectClass) const
{
	const FRavenPool* Pool = GetPoolForClass(ObjectClass);
	return Pool ? Pool->GetActiveCount() : 0;
}

int32 URavenPoolSubsystem::GetInactiveCount(UClass* ObjectClass) const
{
	const FRavenPool* Pool = GetPoolForClass(ObjectClass);
	return Pool ? Pool->GetInactiveCount() : 0;
}

void URavenPoolSubsystem::ClearInactiveObjects(UClass* ObjectClass)
{
	if (FRavenPool* Pool = GetPool(ObjectClass))
	{
		Pool->ClearInactive();
	}
}

void URavenPoolSubsystem::LogPoolStatistics() const
{
	UE_LOG(LogRavenPoolSubsystem, Log, TEXT("=== Pool Statistics ==="));
	UE_LOG(LogRavenPoolSubsystem, Log, TEXT("Total Pools: %d"), Pools.Num());

	for (const FRavenPool& Pool : Pools)
	{
		const int32 ActiveCount = Pool.GetActiveCount();
		const int32 InactiveCount = Pool.GetInactiveCount();
		const int32 TotalCount = Pool.GetPoolSize();
		const float UsagePercent = TotalCount > 0 ? (static_cast<float>(ActiveCount) / TotalCount) * 100.0f : 0.0f;

		UE_LOG(LogRavenPoolSubsystem, Log, TEXT("  [%s] Total: %d | Active: %d | Inactive: %d | Usage: %.1f%% | Max: %s"),
		       Pool.GetObjectClass() ? *Pool.GetObjectClass()->GetName() : TEXT("Unknown"),
		       TotalCount,
		       ActiveCount,
		       InactiveCount,
		       UsagePercent,
		       Pool.GetMaxPoolSize() > 0 ? *FString::FromInt(Pool.GetMaxPoolSize()) : TEXT("Unlimited"));
	}

	UE_LOG(LogRavenPoolSubsystem, Log, TEXT("====================="));
}
