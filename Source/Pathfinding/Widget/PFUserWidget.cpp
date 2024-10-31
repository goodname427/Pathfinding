// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/PFUserWidget.h"
#include "Kismet/GameplayStatics.h"

void UPFUserWidget::PreAddToViewport_Implementation()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (bSetInputModeUIOnly)
	{
		PC->SetInputMode(FInputModeUIOnly().SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways) );
	}
}
