// Fill out your copyright notice in the Description page of Project Settings.


#include "PFGameInstance.h"
#include "WidgetSubsystem.h"
#include "NetworkSubsystem.h"
#include "StartupGameStage.h"
#include "MainMenuGameStage.h"
#include "PFUtils.h"
#include "Kismet/GameplayStatics.h"

void UPFGameInstance::Init()
{
	Super::Init();
	
	CurrentStage = MakeShared<FStartupGameStage>();
}

inline bool UPFGameInstance::IsCurrentStage(const TSharedPtr<IGameStage>& InStage) const
{
	return IGameStage::IsSameStage(CurrentStage, InStage);
}

bool UPFGameInstance::TransitionToStage(const TSharedPtr<IGameStage>& InDesiredStage)
{
	if (!InDesiredStage.IsValid())
	{
		GetSubsystem<UWidgetSubsystem>()->ShowErrorDialog("Error: The Desired State Is Not Valid");
		return false;
	}

	if (IsCurrentStage(InDesiredStage))
	{
		GetSubsystem<UWidgetSubsystem>()->ShowErrorDialog("Error: Can Not Transition To The Same State");
		return false;
	}

	CurrentStage->OnExitStage(this);
	InDesiredStage->OnEnterStage(this);

	CurrentStage = InDesiredStage;

	return true;
}

bool UPFGameInstance::IsCurrentStage(const FString& InStageName) const
{
	return IGameStage::IsSameStage(CurrentStage, InStageName);
}

bool UPFGameInstance::TransitionToStage(const FString& InStageName)
{
	return TransitionToStage(IGameStage::NameToStage(InStageName));
}
