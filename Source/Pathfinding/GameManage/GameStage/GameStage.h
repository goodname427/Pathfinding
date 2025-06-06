#pragma once

class FRoomGameStage;
class FMainMenuGameStage;
class FStartupGameStage;

class UPFGameInstance;

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
	virtual bool CanTransition(UPFGameInstance* GameInstance, FString& OutErrorMessage) { return true; } 
	virtual void OnExitStage(UPFGameInstance* GameInstance) {};
	virtual void OnEnterStage(UPFGameInstance* GameInstance) {};
	virtual void OnPostLogin(UPFGameInstance* GameInstance, APlayerController* NewPlayer) {};
	virtual void OnWorldChanged(UPFGameInstance* GameInstance, UWorld* OldWorld, UWorld* NewWorld) {};
	virtual void OnWorldBeginPlay(UPFGameInstance* GameInstance, UWorld* World) {};

public:
	static bool IsSameStage(const TSharedPtr<IGameStage>& Stage1, const TSharedPtr<IGameStage>& Stage2);

	template<typename TGameStage>
	static bool IsSameStage(const TSharedPtr<IGameStage>& Stage) { return IsSameStage(Stage, TGameStage().GetStageName()); }

	static bool IsSameStage(const TSharedPtr<IGameStage>& Stage, FName StageName);
};


