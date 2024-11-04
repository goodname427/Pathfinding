// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFGameInstance.h"

/**
 *
 */
class PATHFINDING_API IGameStage
{
protected:
	IGameStage() {}
	virtual ~IGameStage() {}

public:
	virtual FString GetStageName() const = 0;
	virtual void OnExitStage(class UPFGameInstance* GameInstance) = 0;
	virtual void OnEnterStage(class UPFGameInstance* GameInstance) = 0;

public:
	static bool IsSameStage(const TSharedPtr<IGameStage>& InStage1, const TSharedPtr<IGameStage>& InStage2);

	template<typename TGameStage>
	static bool IsSameStage(const TSharedPtr<IGameStage> InStage) { return dynamic_cast<TGameStage>(InStage.Get()); }

	static bool IsSameStage(const TSharedPtr<IGameStage>& InStage, const FString& InStageName);

	static TSharedPtr<IGameStage> NameToStage(const FString& InStageName);
};
