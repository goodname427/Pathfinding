// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuGameStage.h"
#include "WidgetSubsystem.h"
#include "NetworkSubsystem.h"
#include "Kismet/GameplayStatics.h"

void FMainMenuGameStage::OnExitStage(UPFGameInstance* GameInstance)
{
}

void FMainMenuGameStage::OnEnterStage(UPFGameInstance* GameInstance)
{
	if (GameInstance->IsCurrentStage("Playing"))
	{
		UGameplayStatics::OpenLevel(GameInstance, TEXT("L_MainMenu"));
	}
	
	UWidgetSubsystem* WidgetSubsystem = GameInstance->GetSubsystem<UWidgetSubsystem>();

	if (WidgetSubsystem)
	{
		WidgetSubsystem->Show(TEXT("MainMenu"));
	}
}
