// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetSubsystem.h"
#include "PFUtils.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "PFUserWidget.h"
#include "WidgetSettings.h"

#define EXECUTE_INTERFACE(Widget, FuncName) \
if (Widget->Implements<UPFUserWidget>()) \
{ \
	IPFUserWidget::Execute_##FuncName(Widget); \
}

void UWidgetSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	GameInstance = Cast<UPFGameInstance>(GetGameInstance());
}

UUserWidget* UWidgetSubsystem::TryGetWidget(FName WidgetName)
{
	NULL_CHECK_RET(GameInstance, nullptr);
	// UE_LOG_TEMP(TEXT("%s"), *InWidgetName);

	UUserWidget*& TargetWidgetRef = WidgetCache.FindOrAdd(WidgetName);
	if (!TargetWidgetRef)
	{
		// Create Widget
		const TSubclassOf<UUserWidget>* TargetWidgetClassPtr = GetDefault<UWidgetSettings>()->WidgetClasses.
			Find(WidgetName);
		if (TargetWidgetClassPtr)
		{
			TargetWidgetRef = CreateWidget(GetWorld(), *TargetWidgetClassPtr, FName(WidgetName));
		}
	}
	// else
	// {
	// 	DEBUG_MESSAGE(TEXT("Widget %s already exists"), *WidgetName.ToString());
	// }

	return TargetWidgetRef;
}

bool UWidgetSubsystem::Push(UUserWidget* TargetWidget)
{
	if (TargetWidget == nullptr || Widgets.Contains(TargetWidget))
	{
		return false;
	}

	// pause current widget
	UUserWidget* CurrentWidget = GetCurrentWidget();
	if (CurrentWidget != nullptr)
	{
		EXECUTE_INTERFACE(CurrentWidget, OnPause);
		CurrentWidget->SetIsEnabled(false);
	}

	// add target widget to viewport
	Widgets.Add(TargetWidget);
	TargetWidget->AddToViewport();
	EXECUTE_INTERFACE(TargetWidget, OnAddToViewport);
	Focus(TargetWidget);

	return true;
}

void UWidgetSubsystem::Focus(UUserWidget* TargetWidget) const
{
	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}
	
	APlayerController* PC = GameInstance->GetFirstLocalPlayerController();// World->GetFirstPlayerController();
	if (PC == nullptr)
	{
		return;
	}
	
	PC->SetShowMouseCursor(true);
	if (TargetWidget->bIsFocusable)
	{
		PC->SetInputMode(FInputModeUIOnly().SetWidgetToFocus(TargetWidget->TakeWidget()));
	}
	else
	{
		PC->SetInputMode(FInputModeGameAndUI());
	}
}

bool UWidgetSubsystem::Push(FName WidgetName)
{
	UUserWidget* TargetWidget = TryGetWidget(WidgetName);

	return Push(TargetWidget);
}

bool UWidgetSubsystem::Pop()
{
	UUserWidget* CurrentWidget = GetCurrentWidget();
	if (CurrentWidget == nullptr)
	{
		return false;
	}

	// remove current widget from viewport
	EXECUTE_INTERFACE(CurrentWidget, OnRemoveFromViewport);
	CurrentWidget->RemoveFromViewport();
	Widgets.Remove(CurrentWidget);

	// resume under widget
	CurrentWidget = GetCurrentWidget();
	if (CurrentWidget != nullptr)
	{
		CurrentWidget->SetIsEnabled(true);
		EXECUTE_INTERFACE(CurrentWidget, OnResume);
		Focus(CurrentWidget);
	}

	return true;
}

void UWidgetSubsystem::ClearUntil(FName WidgetName)
{
	while (!IsCurrentWidget(WidgetName))
	{
		Pop();
	}
}

void UWidgetSubsystem::Clear()
{
	while (Widgets.Num() > 0)
	{
		Pop();
	}
}

void UWidgetSubsystem::ShowErrorDialog(const FString& ErrorText)
{
	// todo
	// Push(TEXT("ErrorDialog"));
}

void UWidgetSubsystem::ShowLoadingScreen(const FString& LoadingText)
{
	Push(TEXT("LoadingScreen"));
}

UUserWidget* UWidgetSubsystem::GetCurrentWidget() const
{
	if (Widgets.Num() > 0)
	{
		return Widgets.Last();
	}

	return nullptr;
}

bool UWidgetSubsystem::IsCurrentWidget(FName WidgetName) const
{
	return WidgetCache.Contains(WidgetName) && WidgetCache[WidgetName] == GetCurrentWidget();
}

FString UWidgetSubsystem::GetCurrentWidgetName() const
{
	UUserWidget* CurrentWidget = GetCurrentWidget();
	return CurrentWidget ? CurrentWidget->GetName() : "None";
}
