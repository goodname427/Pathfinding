// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFGameSession.h"
#include "Engine/GameInstance.h"
#include "Blueprint/UserWidget.h"
#include "GameStage/GameStage.h"
#include "PFGameInstance.generated.h"

class IGameStage;




/**
 * 
 */
UCLASS(Config = Game, DefaultConfig)
class PATHFINDING_API UPFGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	virtual void OnWorldChanged(UWorld* OldWorld, UWorld* NewWorld) override;

	virtual void OnWorldBeginPlay(UWorld* World);

	void PostLogin(APlayerController* NewPlayer);

private:
	FDelegateHandle OnWorldBeginPlayDelegateHandle;

protected:
	// Return True If The Current Stage Equal The In Stage
	inline bool IsCurrentStage(const TSharedPtr<IGameStage>& InStage) const;

	// Transition To Desired Stage
	bool TransitionToStage(const TSharedPtr<IGameStage>& DesiredStage);

public:
	// Return True If The Current Stage Equal The In Stage
	template <typename TGameStage>
	inline bool IsCurrentStage() const;

	// Return True If The Current Stage Equal The Stage Of Specified Name
	UFUNCTION(BlueprintCallable)
	bool IsCurrentStage(FName InStageName) const;

	UFUNCTION(BlueprintCallable)
	FName GetCurrentStageName() const;

	// Transition To Desired Stage
	template <typename TGameStage, typename... ArgTypes>
	bool TransitionToStage(ArgTypes&&... InArgs) { return TransitionToStage(MakeShared<TGameStage>(InArgs...)); }

private:
	TSharedPtr<IGameStage> CurrentStage;

public:
	static FString GetURL(const FString& LevelPath, const FString& Options = TEXT("")) { return FString::Printf(TEXT("/Game/Maps/%s%s"), *LevelPath, *Options); }

public:
	template <typename FmtType, typename... ArgTypes>
	void Error(const FmtType& ErrorMessageFormat, ArgTypes... InArgs);

public:
	UFUNCTION(BlueprintCallable)
	APFGameSession* GetGameSession() const;
};

template <typename TGameStage>
inline bool UPFGameInstance::IsCurrentStage() const
{
	return IGameStage::IsSameStage<TGameStage>(CurrentStage);
}

template <typename FmtType, typename... ArgTypes>
void UPFGameInstance::Error(const FmtType& ErrorMessageFormat, ArgTypes... InArgs)
{
	const FString ErrorMessage = FString::Printf(ErrorMessageFormat, InArgs...);
	UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMessage);
}
