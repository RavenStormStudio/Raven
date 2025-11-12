// RavenStorm Copyright @ 2025-2025

#pragma once

#include "Modules/ModuleManager.h"

class FRavenModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

public:
	static FRavenModule& Get();
};
