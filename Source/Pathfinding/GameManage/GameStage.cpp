// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStage.h"
#include "MainMenuGameStage.h"
#include "StartupGameStage.h"

#define RET_STAGE_IF_MATCHED(StageName) \
if (InStageName == #StageName) \
	return MakeShared<F##StageName##GameStage>() \

bool IGameStage::IsSameStage(const TSharedPtr<IGameStage>& InStage1, const TSharedPtr<IGameStage>& InStage2)
{
	if (!InStage1.IsValid() || !InStage2.IsValid())
		return false;

	return InStage1->GetStageName() == InStage2->GetStageName();
}

bool IGameStage::IsSameStage(const TSharedPtr<IGameStage>& InStage, const FString& InStageName)
{
	return InStage.IsValid() && InStage->GetStageName() == InStageName;
}

TSharedPtr<IGameStage> IGameStage::NameToStage(const FString& InStageName)
{
	RET_STAGE_IF_MATCHED(Startup);
	RET_STAGE_IF_MATCHED(MainMenu);

	return nullptr;
}
