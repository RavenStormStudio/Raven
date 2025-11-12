// RavenStorm Copyright @ 2025-2025

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "RavenGameInstanceBase.generated.h"

UCLASS()
class RAVEN_API URavenGameInstanceBase : public UGameInstance
{
	GENERATED_BODY()

public:
	URavenGameInstanceBase();

protected:
	virtual void Init() override;
	virtual void Shutdown() override;

public:
	UFUNCTION(BlueprintPure, Category="Raven|GameInstance")
	ULocalPlayer* GetPrimaryLocalPlayer() const;

	UFUNCTION(BlueprintPure, Category="Raven|GameInstance")
	APlayerController* GetPrimaryPlayerController() const;

public:
	template <typename T UE_REQUIRES(std::is_base_of_v<ULocalPlayer, T>)>
	T* GetLocalPlayer(int32 Index) const;

	template <typename T UE_REQUIRES(std::is_base_of_v<ULocalPlayer, T>)>
	T* GetPrimaryLocalPlayer() const;

	template <typename T UE_REQUIRES(std::is_base_of_v<APlayerController, T>)>
	T* GetPlayerController(int32 Index) const;

	template <typename T UE_REQUIRES(std::is_base_of_v<APlayerController, T>)>
	T* GetPrimaryPlayerController() const;
};

template <typename T> requires (!!(std::is_base_of_v<ULocalPlayer, T>)) && UE::Core::Private::BoolIdentityConcept<true>
T* URavenGameInstanceBase::GetLocalPlayer(const int32 Index) const
{
	return Cast<T>(GetLocalPlayerByIndex(Index));
}

template <typename T> requires (!!(std::is_base_of_v<ULocalPlayer, T>)) && UE::Core::Private::BoolIdentityConcept<true>
T* URavenGameInstanceBase::GetPrimaryLocalPlayer() const
{
	return GetLocalPlayer<T>(0);
}

template <typename T> requires (!!(std::is_base_of_v<APlayerController, T>)) && UE::Core::Private::BoolIdentityConcept<true>
T* URavenGameInstanceBase::GetPlayerController(const int32 Index) const
{
	if (const ULocalPlayer* LocalPlayer = GetLocalPlayerByIndex(Index))
	{
		return LocalPlayer->GetPlayerController(GetWorld());
	}
	return nullptr;
}

template <typename T> requires (!!(std::is_base_of_v<APlayerController, T>)) && UE::Core::Private::BoolIdentityConcept<true>
T* URavenGameInstanceBase::GetPrimaryPlayerController() const
{
	return GetPlayerController<T>(0);
}
