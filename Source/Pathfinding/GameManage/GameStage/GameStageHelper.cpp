#include "GameStageHelper.h"

#define IMPLEMENT_TRANSITION_TO_STAGE(StageName, ...)\
	TransitionToStage<F##StageName##GameStage>(GameInstance, ##__VA_ARGS__)

void UGameStageHelper::TransitionToMainMenuStage(UPFGameInstance* GameInstance)
{
	IMPLEMENT_TRANSITION_TO_STAGE(MainMenu);
}

void UGameStageHelper::TransitionToRoomStage(UPFGameInstance* GameInstance, AGameSession* InSessionToJoin)
{
	IMPLEMENT_TRANSITION_TO_STAGE(Room, InSessionToJoin);
}
