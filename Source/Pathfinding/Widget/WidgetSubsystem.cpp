// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetSubsystem.h"

#include "PFGameSettings.h"
#include "PFUtils.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "PFUserWidget.h"
#include "WidgetSettings.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"

#define EXECUTE_INTERFACE(Widget, FuncName) \
if (Widget->Implements<UPFUserWidget>()) \
{ \
	IPFUserWidget::Execute_##FuncName(Widget); \
}

void UWidgetSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	GameInstance = Cast<UPFGameInstance>(GetGameInstance());

	// Load Sound
	// const UWidgetSettings* Settings = GetDefault<UWidgetSettings>();
	// if (Settings)
	// {
	// 	ButtonClickSound = Cast<USoundBase>(Settings->ButtonClickSound.TryLoad());
	// 	WidgetHideSound = Cast<USoundBase>(Settings->WidgetHideSound.TryLoad());
	// 	WidgetShowSound = Cast<USoundBase>(Settings->WidgetShowSound.TryLoad());
	// }
}

void UWidgetSubsystem::BindSounds(UUserWidget* TargetWidget, bool bRepeat)
{
	if (bRepeat)
	{
		if (BindSoundWidgets.Contains(TargetWidget))
		{
			return;
		}
		BindSoundWidgets.Add(TargetWidget);
	}

	TArray<UWidget*> AllChildWidgets;
	// Get all child widgets
	if (TargetWidget->WidgetTree)
	{
		TargetWidget->WidgetTree->GetAllWidgets(AllChildWidgets);
	}

	for (UWidget* Widget : AllChildWidgets)
	{
		if (UButton* Button = Cast<UButton>(Widget))
		{
			// Bind click sound
			if (!Button->OnClicked.IsAlreadyBound(this, &UWidgetSubsystem::PlayButtonClickSound))
			{
				Button->OnClicked.AddDynamic(this, &UWidgetSubsystem::PlayButtonClickSound);
			}
		}
	}
}

void UWidgetSubsystem::PlaySound(USoundBase* Sound)
{
	UGameplayStatics::PlaySound2D(GetWorld(), Sound);
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
	if (TargetWidget == nullptr || WidgetStack.Contains(TargetWidget))
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
	WidgetStack.Add(TargetWidget);
	TargetWidget->AddToViewport();
	// PlaySound(WidgetShowSound);
	EXECUTE_INTERFACE(TargetWidget, OnAddToViewport);
	// BindSounds(TargetWidget);
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

	APlayerController* PC = GameInstance->GetFirstLocalPlayerController(); // World->GetFirstPlayerController();
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

void UWidgetSubsystem::PlayButtonClickSound()
{
	PlaySound(ButtonClickSound);
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
	// PlaySound(WidgetHideSound);
	CurrentWidget->RemoveFromViewport();
	WidgetStack.Remove(CurrentWidget);

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
	while (WidgetStack.Num() > 0)
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
	if (WidgetStack.Num() > 0)
	{
		return WidgetStack.Last();
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
