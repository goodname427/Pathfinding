#include "GameStageHelper.h"
#include "RoomGameStage.h"
#include "MainMenuGameStage.h"
#include "FindRoomGameStage.h"

#define IMPLEMENT_TRANSITION_TO_STAGE(StageName, ...)\
	TransitionToStage<F##StageName##GameStage>(WorldContextObject, ##__VA_ARGS__)

void UGameStageHelper::TransitionToMainMenuStage(UObject* WorldContextObject)
{
	IMPLEMENT_TRANSITION_TO_STAGE(MainMenu);
}

void UGameStageHelper::TransitionToFindRoomStage(UObject* WorldContextObject)
{
	IMPLEMENT_TRANSITION_TO_STAGE(FindRoom);
}

void UGameStageHelper::TransitionToRoomStage(UObject* WorldContextObject, AGameSession* InSessionToJoin)
{
	IMPLEMENT_TRANSITION_TO_STAGE(Room, InSessionToJoin);
}
