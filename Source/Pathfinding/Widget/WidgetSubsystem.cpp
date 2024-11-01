// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetSubsystem.h"
#include "PFUtils.h"
#include "Blueprint/UserWidget.h"

void UWidgetSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	CurrentWidget = nullptr;
	GameInstance = Cast<UPFGameInstance>(GetGameInstance());
}

UUserWidget* UWidgetSubsystem::TryGetWidget(const FString& InWidgetName)
{
	NULL_CHECK_RET(GameInstance, nullptr);
	// UE_LOG_TEMP(TEXT("%s"), *InWidgetName);
	
	UUserWidget*& TargetWidgetRef = LoadedWidgets.FindOrAdd(InWidgetName);
	if (!TargetWidgetRef)
	{
		// 创建并初始化小部件
		TSubclassOf<UUserWidget>* TargetWidgetClassPtr = GameInstance->WidgetSettings.Find(InWidgetName);
		if (TargetWidgetClassPtr)
		{
			TargetWidgetRef = CreateWidget(GetWorld(), *TargetWidgetClassPtr);
		}
	}

	return TargetWidgetRef;
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
		InTargetWidget->AddToViewport();
		CurrentWidget = InTargetWidget;
		return;
	}
}

void UWidgetSubsystem::Show(const FString& InWidgetName)
{
	// UE_LOG_TEMP(TEXT("Show"));
	UUserWidget* TargetWidget = TryGetWidget(InWidgetName);
	// UE_LOG_TEMP(TEXT("%s"), TargetWidget ? *TargetWidget->GetFName().ToString() : TEXT("NULL"));
	Show(TargetWidget);
}

void UWidgetSubsystem::Clear()
{
	Show(nullptr);
}

void UWidgetSubsystem::ShowErrorDialog(const FString& ErrorText)
{
	// Show(TEXT("ErrorDialog"));
}
