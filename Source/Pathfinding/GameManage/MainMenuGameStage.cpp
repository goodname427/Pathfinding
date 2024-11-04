// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuGameStage.h"
#include "WidgetSubsystem.h"
#include "NetworkSubsystem.h"
#include "PFUtils.h"
#include "Kismet/GameplayStatics.h"

void FMainMenuGameStage::OnExitStage(UPFGameInstance* GameInstance)
{
}

void FMainMenuGameStage::OnEnterStage(UPFGameInstance* GameInstance)
{
	// UE_LOG_TEMP(TEXT("Current Level Name: %s"), *GameInstance->GetWorld()->GetName());
	if (GameInstance->GetWorld()->GetName() != TEXT("L_MainMenu"))
	{
		UGameplayStatics::OpenLevel(GameInstance, TEXT("L_MainMenu"));
	}
	
	GameInstance->GetSubsystem<UWidgetSubsystem>()->Show(TEXT("MainMenu"));
}
