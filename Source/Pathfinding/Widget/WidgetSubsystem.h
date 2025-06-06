// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameManage/PFGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "WidgetSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class PATHFINDING_API UWidgetSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

private:
	UPROPERTY()
	UPFGameInstance* GameInstance;

public:
	UFUNCTION(BlueprintCallable)
	void BindSounds(UUserWidget* TargetWidget, bool bRepeat = true);

	UFUNCTION(BlueprintCallable)
	void PlaySound(USoundBase* Sound);
	
	UPROPERTY()
	USoundBase* ButtonClickSound;
	UFUNCTION(BlueprintCallable)
	void PlayButtonClickSound();

	UPROPERTY()
	USoundBase* WidgetShowSound;
	UPROPERTY()
	USoundBase* WidgetHideSound;

private:
	UPROPERTY()
	TSet<UUserWidget*> BindSoundWidgets;

protected:
	// Try To Get Widget Of The Specified Name
	UUserWidget* TryGetWidget(FName WidgetName);
	// Push The Specified Widget And Hide The Current Widget
	bool Push(UUserWidget* TargetWidget);

	void Focus(UUserWidget* TargetWidget) const;

public:
	// Push Widget Of The Specified Name And Hide The Current Widget
	UFUNCTION(BlueprintCallable)
	bool Push(FName WidgetName);

	UFUNCTION(BlueprintCallable)
	bool Pop();

	UFUNCTION(BlueprintCallable)
	void ClearUntil(FName WidgetName);

	UFUNCTION(BlueprintCallable)
	void Clear();

	// Push Error Dialog
	UFUNCTION(BlueprintCallable)
	void ShowErrorDialog(const FString& ErrorText);

	UFUNCTION(BlueprintCallable)
	void ShowLoadingScreen(const FString& LoadingText = TEXT(""));

	UFUNCTION(BlueprintCallable)
	UUserWidget* GetCurrentWidget() const;

	UFUNCTION(BlueprintCallable)
	bool IsCurrentWidget(FName WidgetName) const;

	UFUNCTION(BlueprintCallable)
	FString GetCurrentWidgetName() const;

private:
	UPROPERTY()
	TArray<UUserWidget*> WidgetStack;

	// The Cache of All Loaded Widgets
	UPROPERTY()
	TMap<FName, UUserWidget*> WidgetCache;
};
