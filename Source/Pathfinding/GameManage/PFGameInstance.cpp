// Fill out your copyright notice in the Description page of Project Settings.


#include "PFGameInstance.h"
#include "WidgetSubsystem.h"
#include "PFGameSession.h"
#include "GameStage/StartupGameStage.h"
#include "PFUtils.h"
#include "GameFramework/GameModeBase.h"
#include "GameStage/GameStageHelper.h"
#include "Kismet/GameplayStatics.h"

void UPFGameInstance::Init()
{
	Super::Init();
	
	TransitionToStage<FStartupGameStage>();
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

	if (CurrentStage.IsValid())
	{
		CurrentStage->OnExitStage(this);
	}
	
	DesiredStage->OnEnterStage(this);

	CurrentStage = DesiredStage;

	return true;
}

bool UPFGameInstance::IsCurrentStage(FName InStageName) const
{
	return IGameStage::IsSameStage(CurrentStage, InStageName);
}

void UPFGameInstance::Error(const FString& ErrorMessage)
{
	UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMessage);
}

APFGameSession* UPFGameInstance::GetGameSession() const
{
	UWorld* World = GetWorld();
	if (World)
	{
		AGameModeBase* GameMode = World->GetAuthGameMode();
		if (GameMode)
		{
			return Cast<APFGameSession>(GameMode->GameSession);
		}
	}

	return nullptr;
}
