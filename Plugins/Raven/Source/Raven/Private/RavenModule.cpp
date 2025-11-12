// RavenStorm Copyright @ 2025-2025

#include "RavenModule.h"

#include "RavenGlobals.h"

#define LOCTEXT_NAMESPACE "FRavenModule"

void FRavenModule::StartupModule()
{
	UE_LOGFMT(LogRaven, Log, "Raven Module loaded");
}

void FRavenModule::ShutdownModule()
{
	UE_LOGFMT(LogRaven, Log, "Raven Module unloaded");
}

FRavenModule& FRavenModule::Get()
{
	return FModuleManager::LoadModuleChecked<FRavenModule>(TEXT("Raven"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FRavenModule, Raven)
