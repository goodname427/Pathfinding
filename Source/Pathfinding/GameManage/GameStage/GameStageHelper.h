#pragma once

#include "GameStage.h"
#include "GameStageHelper.generated.h"

#define DECLARE_TRANSITION_TO_STAGE(StageName, ...) \
	UFUNCTION(BlueprintCallable) \
	static void TransitionTo##StageName##Stage(UPFGameInstance* GameInstance, ##__VA_ARGS__)

UCLASS()
class PATHFINDING_API UGameStageHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	template <typename TGameStage, typename... ArgTypes>
	static void TransitionToStage(UObject* WorldContextObject, ArgTypes&&... InArgs);

public:
	// Blueprint Help

	UFUNCTION(BlueprintCallable)
	static void TransitionToMainMenuStage(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable)
	static void TransitionToFindRoomStage(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable)
	static void TransitionToRoomStage(UObject* WorldContextObject, AGameSession* InSessionToJoin);
};


template <typename TGameStage, typename... ArgTypes>
void UGameStageHelper::TransitionToStage(UObject* WorldContextObject, ArgTypes&&... InArgs)
{
	UPFGameInstance* CurrentGameInstance = Cast<UPFGameInstance>(WorldContextObject->GetWorld()->GetGameInstance());
	if (CurrentGameInstance)
	{
		CurrentGameInstance->TransitionToStage<TGameStage, ArgTypes...>(InArgs...);
	}
}
