// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFGameSettings.h"
#include "GameFramework/PlayerController.h"
#include "PFPlayerController.generated.h"

#define IMPLEMENT_TRANSITION_TO_STAGE(StageName, ...) \
{ \
	UPFGameInstance* GI = Cast<UPFGameInstance>(GetGameInstance()); \
	if (GI) \
	{ \
		GI->TransitionToStage<F##StageName##GameStage>(__VA_ARGS__); \
	} \
}

#define IMPLEMENT_ALL_TRANSITION_TO_STAGE(StageName, ...) \
{ \
	if (GetLocalRole() == ROLE_Authority) \
	{ \
		for (auto PCIter = GetWorld()->GetPlayerControllerIterator(); PCIter; ++PCIter) \
		{ \
			APFPlayerController* PF = Cast<APFPlayerController>(*PCIter); \
			if (PF) \
			{ \
				PF->TransitionTo##StageName##Stage(__VA_ARGS__); \
			} \
		} \
	} \
}


/**
 * 
 */
UCLASS()
class PATHFINDING_API APFPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	APFPlayerController();
	
protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	void AllTransitionToMainMenuStage();
	UFUNCTION(BlueprintCallable, Client, Reliable)
	void TransitionToMainMenuStage();
	
	UFUNCTION(BlueprintCallable, Client, Reliable)
	void TransitionToRoomStageByJoin(int32 InRoomIndexToJoin);
	UFUNCTION(BlueprintCallable, Client, Reliable)
	void TransitionToRoomStageByHost(FMapInfo InMapInfo);
	
	UFUNCTION(BlueprintCallable)
	void AllTransitionToPlayingStage(const FString& InLevelPathToPlay);
	UFUNCTION(BlueprintCallable, Client, Reliable)
	void TransitionToPlayingStage(const FString& InLevelPathToPlay);
};
