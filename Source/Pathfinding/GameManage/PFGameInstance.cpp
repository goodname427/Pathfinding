// Fill out your copyright notice in the Description page of Project Settings.


#include "PFGameInstance.h"
#include "WidgetSubsystem.h"
#include "NetworkSubsystem.h"
#include "GameStage/StartupGameStage.h"
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

bool UPFGameInstance::TransitionToStage(const TSharedPtr<IGameStage>& DesiredStage)
{
	if (!DesiredStage.IsValid())
	{
		Error(TEXT("Error: The Desired State Is Not Valid"));
		return false;
	}

	if (IsCurrentStage(DesiredStage))
	{
		Error(TEXT("Error: Can Not Transition To The Same State"));
		return false;
	}

	CurrentStage->OnExitStage(this);
	DesiredStage->OnEnterStage(this);

	CurrentStage = DesiredStage;

	return true;
}

bool UPFGameInstance::IsCurrentStage(const FString& InStageName) const
{
	return IGameStage::IsSameStage(CurrentStage, InStageName);
}

bool UPFGameInstance::TransitionToStage(const FString& DesiredStageName)
{
	return TransitionToStage(IGameStage::NameToStage(DesiredStageName));
}

void UPFGameInstance::Error(const FString& ErrorMessage)
{
	UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMessage);
}
