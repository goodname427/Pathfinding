// Fill out your copyright notice in the Description page of Project Settings.


#include "PFEngine.h"

#include "PFUtils.h"
#include "GameStage/MainMenuGameStage.h"

class UPFGameInstance;

void UPFEngine::Init(IEngineLoop* InEngineLoop)
{
	Super::Init(InEngineLoop);
}

void UPFEngine::HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType,
                                     const FString& ErrorString)
{
	Super::HandleNetworkFailure(World, NetDriver, FailureType, ErrorString);

	// DEBUG_MESSAGE(TEXT("HandleNetworkFailure"));
	// UPFGameInstance* GI = Cast<UPFGameInstance>(World->GetGameInstance());
	// if (GI)
	// {
	// 	GI->TransitionToStage<FMainMenuGameStage>();
	// }
}
