#include "GameStage.h"
#include "MainMenuGameStage.h"
#include "StartupGameStage.h"
#include "RoomGameStage.h"

#define RET_STAGE_IF_MATCHED(StageName) \
if (InStageName == #StageName) \
	return MakeShared<F##StageName##GameStage>() \

TSharedPtr<IGameStage> IGameStage::NameToStage(const FName& InStageName)
{
	RET_STAGE_IF_MATCHED(Startup);
	RET_STAGE_IF_MATCHED(MainMenu);
	RET_STAGE_IF_MATCHED(Room);

	return nullptr;
}

bool IGameStage::IsSameStage(const TSharedPtr<IGameStage>& Stage1, const TSharedPtr<IGameStage>& Stage2)
{
	if (!Stage1.IsValid() || !Stage2.IsValid())
		return false;

	return Stage1->GetStageName() == Stage2->GetStageName();
}

bool IGameStage::IsSameStage(const TSharedPtr<IGameStage>& Stage, const FName& StageName)
{
	return Stage.IsValid() && Stage->GetStageName() == StageName;
}
