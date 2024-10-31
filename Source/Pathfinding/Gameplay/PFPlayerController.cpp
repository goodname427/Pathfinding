// Fill out your copyright notice in the Description page of Project Settings.


#include "PFPlayerController.h"

void APFPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	//SetInputMode(FInputModeGameAndUI().SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways));
	SetShowMouseCursor(true);
}
