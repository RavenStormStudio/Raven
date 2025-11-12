// RavenStorm Copyright @ 2025-2025

#include "Pool/Factory/RavenPoolActorFactory.h"

UObject* URavenPoolActorFactory::CreatePoolObject(UClass* Class)
{
	UWorld* World = GetWorld();

	FActorSpawnParameters SpawnParameters = {};
	SpawnParameters.OverrideLevel = World->PersistentLevel;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.bDeferConstruction = false;
	SpawnParameters.bNoFail = true;
#if WITH_EDITORONLY_DATA
	SpawnParameters.bCreateActorPackage = false;
#endif

	const FTransform Transform = FTransform::Identity;
	return World->SpawnActor(Class, &Transform, SpawnParameters);
}

void URavenPoolActorFactory::DestroyPoolObject(UObject* Object)
{
	AActor* Actor = CastChecked<AActor>(Object);
	Actor->Destroy();
}

void URavenPoolActorFactory::PrepareForStorage(UObject* Object)
{
	Super::PrepareForStorage(Object);

	AActor* Actor = CastChecked<AActor>(Object);
	Actor->SetActorLocation(FVector(0.0f, 0.0f, -10000.0f));
	Actor->SetActorHiddenInGame(true);
	Actor->SetActorEnableCollision(false);
	Actor->SetActorTickEnabled(false);
}

void URavenPoolActorFactory::PrepareForUsage(UObject* Object)
{
	Super::PrepareForUsage(Object);

	AActor* Actor = CastChecked<AActor>(Object);
	// Note: Location should be set by the caller after acquiring the actor
	Actor->SetActorHiddenInGame(false);
	Actor->SetActorEnableCollision(true);
	Actor->SetActorTickEnabled(true);
}
