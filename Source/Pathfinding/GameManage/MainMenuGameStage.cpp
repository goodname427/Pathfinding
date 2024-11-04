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
	if (GameInstance->GetWorld()->PersistentLevel->GetFName() != TEXT("L_MainMenu"))
	{
		UGameplayStatics::OpenLevel(GameInstance, TEXT("L_MainMenu"));
	}
	
	GameInstance->GetSubsystem<UWidgetSubsystem>()->Show(TEXT("MainMenu"));
}
