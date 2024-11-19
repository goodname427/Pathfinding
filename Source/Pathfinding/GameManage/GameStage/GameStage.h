#pragma once

#include "PFGameInstance.h"

class FRoomGameStage;
class FMainMenuGameStage;
class FStartupGameStage;

/**
 *
 */
class PATHFINDING_API IGameStage
{
protected:
	IGameStage() {}
	virtual ~IGameStage() {}

public:
	virtual FName GetStageName() const = 0;
	virtual bool CanTransition(class UPFGameInstance* GameInstance) { return true; } 
	virtual void OnExitStage(class UPFGameInstance* GameInstance) {};
	virtual void OnEnterStage(class UPFGameInstance* GameInstance) {};
	virtual void OnPostLogin(class UPFGameInstance* GameInstance, APlayerController* NewPlayer) {};
	virtual void OnWorldChanged(class UPFGameInstance* GameInstance, UWorld* OldWorld, UWorld* NewWorld) {};
	virtual void OnWorldBeginPlay(class UPFGameInstance* GameInstance, UWorld* World) {};

public:
	static bool IsSameStage(const TSharedPtr<IGameStage>& Stage1, const TSharedPtr<IGameStage>& Stage2);

	template<typename TGameStage>
	static bool IsSameStage(const TSharedPtr<IGameStage>& Stage) { return StaticCastSharedPtr<TGameStage>(Stage).IsValid(); }

	static bool IsSameStage(const TSharedPtr<IGameStage>& Stage, FName StageName);
};


