#pragma once
#include "GameStage.h"

class PATHFINDING_API FSettlementGameStage final : public IGameStage
{
public:
	FSettlementGameStage() : bFailed(false) {};
	explicit FSettlementGameStage(bool bInFailed) : bFailed(bInFailed) {};
	
public:
	virtual FName GetStageName() const override { return FName("Settlement"); }

	virtual bool CanTransition(UPFGameInstance* GameInstance, FString& OutErrorMessage) override;

	virtual void OnEnterStage(UPFGameInstance* GameInstance) override;

	virtual void OnExitStage(UPFGameInstance* GameInstance) override;

	virtual void OnWorldBeginPlay(UPFGameInstance* GameInstance, UWorld* World) override;

private:
	bool bFailed;
	
private:
	static FName LevelName;
	static FName WidgetName;
};
