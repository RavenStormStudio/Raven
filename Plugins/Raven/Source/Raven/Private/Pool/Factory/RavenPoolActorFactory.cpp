// RavenStorm Copyright @ 2025-2025

#include "Pool/Factory/RavenPoolActorFactory.h"
#include "Pool/RavenPoolTypes.h"
#include "Pool/RavenPoolStats.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

UObject* URavenPoolActorFactory::CreatePoolObject_Implementation(UClass* Class)
{
	SCOPE_CYCLE_COUNTER(STAT_ActorFactory_Create);

	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.OverrideLevel = World->PersistentLevel;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.bDeferConstruction = false;
	SpawnParameters.bNoFail = true;
#if WITH_EDITORONLY_DATA
	SpawnParameters.bCreateActorPackage = false;
#endif

	// Spawn at storage location initially
	const FTransform Transform(FRotator::ZeroRotator, StorageLocation);
	AActor* Actor = World->SpawnActor(Class, &Transform, SpawnParameters);

	// Immediately prepare for storage
	if (Actor)
	{
		Actor->SetActorHiddenInGame(true);
		Actor->SetActorEnableCollision(false);
		Actor->SetActorTickEnabled(false);
	}

	return Actor;
}

UObject* URavenPoolActorFactory::CreatePoolObjectWithContext_Implementation(const FPoolCreationContext& Context)
{
	// For actors, we can use the same logic regardless of context
	return CreatePoolObject(Context.ObjectClass);
}

void URavenPoolActorFactory::DestroyPoolObject_Implementation(UObject* Object)
{
	SCOPE_CYCLE_COUNTER(STAT_ActorFactory_Destroy);

	if (AActor* Actor = Cast<AActor>(Object))
	{
		Actor->Destroy();
	}
	else
	{
		Super::DestroyPoolObject_Implementation(Object);
	}
}

void URavenPoolActorFactory::PrepareForStorage_Implementation(UObject* Object)
{
	SCOPE_CYCLE_COUNTER(STAT_ActorFactory_PrepareStorage);

	Super::PrepareForStorage_Implementation(Object);

	AActor* Actor = Cast<AActor>(Object);
	if (!Actor)
	{
		return;
	}

	Actor->SetActorLocation(StorageLocation);
	Actor->SetActorHiddenInGame(true);
	Actor->SetActorEnableCollision(false);
	Actor->SetActorTickEnabled(false);

	// Optionally disable components
	if (bDisableComponents)
	{
		TArray<UActorComponent*> Components;
		Actor->GetComponents(Components);
		for (UActorComponent* Component : Components)
		{
			if (Component && Component->IsActive())
			{
				Component->Deactivate();
			}
		}
	}
}

void URavenPoolActorFactory::PrepareForUsage_Implementation(UObject* Object)
{
	SCOPE_CYCLE_COUNTER(STAT_ActorFactory_PrepareUsage);

	Super::PrepareForUsage_Implementation(Object);

	AActor* Actor = Cast<AActor>(Object);
	if (!Actor)
	{
		return;
	}

	// Note: Location should be set by the caller after acquiring the actor
	Actor->SetActorHiddenInGame(false);
	Actor->SetActorEnableCollision(true);
	Actor->SetActorTickEnabled(true);

	// Optionally re-enable components
	if (bDisableComponents)
	{
		TArray<UActorComponent*> Components;
		Actor->GetComponents(Components);
		for (UActorComponent* Component : Components)
		{
			if (Component && !Component->IsActive())
			{
				Component->Activate();
			}
		}
	}
}

bool URavenPoolActorFactory::CanCreateClass_Implementation(UClass* Class) const
{
	return IsValid(Class) && Class->IsChildOf(AActor::StaticClass());
}
