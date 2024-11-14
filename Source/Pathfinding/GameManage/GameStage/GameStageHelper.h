#pragma once

#include "GameStage.h"
#include "StartupGameStage.h"
#include "RoomGameStage.h"
#include "MainMenuGameStage.h"
#include "GameStageHelper.generated.h"

#define DECLARE_TRANSITION_TO_STAGE(StageName, ...) \
	UFUNCTION(BlueprintCallable) \
	static void TransitionTo##StageName##Stage(UPFGameInstance* GameInstance, ##__VA_ARGS__)

UCLASS()
class PATHFINDING_API UGameStageHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	template <typename TGameStage, typename... ArgTypes>
	static void TransitionToStage(UPFGameInstance* GameInstance, ArgTypes&&... InArgs);

public:
	// Blueprint Help

	UFUNCTION(BlueprintCallable)
	static void TransitionToMainMenuStage(UPFGameInstance* GameInstance);

	UFUNCTION(BlueprintCallable)
	static void TransitionToRoomStage(UPFGameInstance* GameInstance, AGameSession* InSessionToJoin);
};


template <typename TGameStage, typename... ArgTypes>
void UGameStageHelper::TransitionToStage(UPFGameInstance* GameInstance, ArgTypes&&... InArgs)
{
	GameInstance->TransitionToStage<TGameStage, ArgTypes...>(InArgs...);
}
