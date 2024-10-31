// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetSubsystem.h"
#include "PFGameInstance.h"
#include "PFUtils.h"
#include "Blueprint/UserWidget.h"

void UWidgetSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	GameInstance = Cast<UPFGameInstance>(GetGameInstance());
}

UUserWidget* UWidgetSubsystem::TryGetWidget(FString InWidgetName)
{
	NULL_CHECK_RET(GameInstance, nullptr);

	UUserWidget** TargetWidgetPtr = LoadedWidgets.Find(InWidgetName);
	
	if (TargetWidgetPtr == nullptr)
	{ 
		TSubclassOf<UUserWidget>* TargetWidgetClassPtr = GameInstance->WidgetSettings.Find(InWidgetName);
		if (TargetWidgetClassPtr)
		{
			UUserWidget* TargetWidget = CreateWidget(GetWorld(), *TargetWidgetClassPtr);
			TargetWidgetPtr = &TargetWidget;
			if (TargetWidget != nullptr)
			{
				LoadedWidgets.Add(InWidgetName, TargetWidget);
			}
		}
	}

	return TargetWidgetPtr ? *TargetWidgetPtr : nullptr;
}

void UWidgetSubsystem::Show(UUserWidget* InTargetWidget)
{
	if (CurrentWidget != nullptr)
	{
		CurrentWidget->RemoveFromViewport();
		CurrentWidget = nullptr;
	}

	if (InTargetWidget != nullptr)
	{
		// InTargetWidget->PreAddToViewport();
		InTargetWidget->AddToViewport();
		CurrentWidget = InTargetWidget;
		return;
	}
}

void UWidgetSubsystem::Show(FString InWidgetName)
{
	UE_LOG_TEMP(TEXT("Show"));
	UUserWidget* TargetWidget = TryGetWidget(InWidgetName);
	UE_LOG_TEMP(TEXT("%s"), TargetWidget ? *TargetWidget->GetFName().ToString() : TEXT("NULL"));
	Show(TargetWidget);
}

void UWidgetSubsystem::Clear()
{
	Show(nullptr);
}

void UWidgetSubsystem::ShowErrorDialog(FString ErrorText)
{
	Show(TEXT("ErrorDialog"));
}
