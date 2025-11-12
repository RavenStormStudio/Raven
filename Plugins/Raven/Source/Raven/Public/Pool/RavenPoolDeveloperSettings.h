// RavenStorm Copyright @ 2025-2025

#pragma once

#include "CoreMinimal.h"
#include "RavenPoolTypes.h"

#include "Engine/DeveloperSettings.h"

#include "Factory/RavenPoolFactoryUObject.h"

#include "RavenPoolDeveloperSettings.generated.h"

/**
 * Configuration for a single object pool.
 * Defines which class should be pooled and which factory to use for creating instances.
 */
USTRUCT(BlueprintType)
struct RAVEN_API FRavenPoolConfig
{
	GENERATED_BODY()

	/** The class of objects to pool */
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Basic", meta=(BlueprintProtected = "true"))
	TObjectPtr<UClass> Class;

	/** The factory class to use for creating and managing pooled objects */
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Basic", meta=(BlueprintProtected = "true"))
	TSoftClassPtr<URavenPoolFactoryUObject> Factory;

	/** Initial number of objects to pre-create in the pool (0 = no pre-warming) */
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Size", meta=(BlueprintProtected = "true", ClampMin = "0"))
	int32 InitialPoolSize = 0;

	/** Maximum number of objects allowed in the pool (0 = unlimited) */
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Size", meta=(BlueprintProtected = "true", ClampMin = "0"))
	int32 MaxPoolSize = 0;

	/** Pool management policy */
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Policy", meta=(BlueprintProtected = "true"))
	FRavenPoolPolicy Policy;
};

/**
 * Developer settings for configuring object pools.
 * Define which classes should be pooled and their factories.
 */
UCLASS(Config = "RavenPool", DefaultConfig, meta = (DisplayName = "Pool"))
class RAVEN_API URavenPoolDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

protected:
	virtual FName GetContainerName() const override;
	virtual FName GetCategoryName() const override;

public:
	/**
	 * Gets the configured pool configurations.
	 * @return Array of pool configurations
	 */
	UFUNCTION(BlueprintPure, Category="Raven|Pool")
	const TArray<FRavenPoolConfig>& GetPoolConfigs() const;

protected:
	/** Array of pool configurations defining which classes to pool and their factories */
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (BlueprintProtected = "true"))
	TArray<FRavenPoolConfig> PoolConfigs;
};
