﻿#pragma once

#include "GameStage.h"

class PATHFINDING_API FPlayingGameStage final : public IGameStage
{
public:
	FPlayingGameStage() : FPlayingGameStage(TEXT("")) {};
	explicit  FPlayingGameStage(const FString& InLevelPathToPlay)
		: LevelPathToPlay(InLevelPathToPlay)
	{
	}

	virtual FName GetStageName() const override { return "Playing"; }
	virtual bool CanTransition(UPFGameInstance* GameInstance, FString& OutErrorMessage) override;
	virtual void OnEnterStage(class UPFGameInstance* GameInstance) override;
	virtual void OnExitStage(class UPFGameInstance* GameInstance) override;
	virtual void OnWorldBeginPlay(class UPFGameInstance* GameInstance, UWorld* World) override;

private:
	FString LevelPathToPlay;

private:
	static FName WidgetName;
};