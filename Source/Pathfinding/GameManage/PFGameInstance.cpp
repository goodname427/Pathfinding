// Fill out your copyright notice in the Description page of Project Settings.


#include "PFGameInstance.h"
#include "WidgetSubsystem.h"
#include "PFGameSession.h"
#include "GameStage/StartupGameStage.h"
#include "PFUtils.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"

void UPFGameInstance::Init()
{
	Super::Init();

	TransitionToStage<FStartupGameStage>();
}

void UPFGameInstance::OnWorldChanged(UWorld* OldWorld, UWorld* NewWorld)
{
	Super::OnWorldChanged(OldWorld, NewWorld);
	if (OldWorld != nullptr && OnWorldBeginPlayDelegateHandle.IsValid())
	{
		OldWorld->OnWorldBeginPlay.Remove(OnWorldBeginPlayDelegateHandle);
	}

	if (NewWorld != nullptr)
	{
		OnWorldBeginPlayDelegateHandle = NewWorld->OnWorldBeginPlay.AddLambda([this, NewWorld]
		{
			OnWorldBeginPlay(NewWorld);
		});
	}

	if (CurrentStage)
	{
		CurrentStage->OnWorldChanged(this, OldWorld, NewWorld);
		// TSharedPtr<FTimerHandle> TimerHandle = MakeShared<FTimerHandle>(); 
		// GetTimerManager().SetTimer(*TimerHandle, [this, TimerHandle, OldWorld, NewWorld]()
		// {
		// 	CurrentStage->OnWorldChanged(this, OldWorld, NewWorld);
		// 	GetTimerManager().ClearTimer(*TimerHandle);
		// }, 0.1f, false);
	}
}

void UPFGameInstance::OnWorldBeginPlay(UWorld* World)
{
	if (CurrentStage)
	{
		CurrentStage->OnWorldBeginPlay(this, World);
	}
}

void UPFGameInstance::PostLogin(APlayerController* NewPlayer)
{
	if (CurrentStage.IsValid())
	{
		CurrentStage->OnPostLogin(this, NewPlayer);
	}
}

inline bool UPFGameInstance::IsCurrentStage(const TSharedPtr<IGameStage>& InStage) const
{
	return IGameStage::IsSameStage(CurrentStage, InStage);
}

bool UPFGameInstance::TransitionToStage(const TSharedPtr<IGameStage>& DesiredStage)
{
	if (!DesiredStage.IsValid())
	{
		Error(TEXT("Error: The Desired State Is Not Valid"));
		return false;
	}

	if (IsCurrentStage(DesiredStage))
	{
		Error(TEXT("Error: Can Not Transition To The Same State [%s]"), *DesiredStage->GetStageName().ToString());
		return false;
	}

	FString ErrorMessage;
	if (!DesiredStage->CanTransition(this, ErrorMessage))
	{
		Error(
			TEXT("Error: Desired Stage [%s] Can Not Transition\n")
			TEXT("ErrorMessage: %s"),
			*DesiredStage->GetStageName().ToString(),
			*ErrorMessage
		);
		return false;
	}

	if (CurrentStage.IsValid())
	{
		CurrentStage->OnExitStage(this);
	}

	DesiredStage->OnEnterStage(this);

	CurrentStage = DesiredStage;

	return true;
}

bool UPFGameInstance::IsCurrentStage(FName InStageName) const
{
	return IGameStage::IsSameStage(CurrentStage, InStageName);
}

FName UPFGameInstance::GetCurrentStageName() const
{
	if (CurrentStage.IsValid())
	{
		return CurrentStage->GetStageName();
	}

	return NAME_None;
}

APFGameSession* UPFGameInstance::GetGameSession() const
{
	UWorld* World = GetWorld();
	if (World)
	{
		AGameModeBase* GameMode = World->GetAuthGameMode();
		if (GameMode)
		{
			return Cast<APFGameSession>(GameMode->GameSession);
		}
	}

	return nullptr;
}
