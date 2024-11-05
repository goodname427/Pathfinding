// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Blueprint/UserWidget.h"
#include "GameStage.h"
#include "PFGameInstance.generated.h"

class IGameStage;

/**
 * 
 */
UCLASS()
class PATHFINDING_API UPFGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TMap<FString, TSubclassOf<UUserWidget>> WidgetSettings;

public:
	virtual void Init() override;

protected:
	// Return True If The Current Stage Equal The In Stage
	inline bool IsCurrentStage(const TSharedPtr<IGameStage>& InStage) const;
	
	// Transition To Desired Stage
	bool TransitionToStage(const TSharedPtr<IGameStage>& DesiredStage);

public:
	// Return True If The Current Stage Equal The In Stage
	template<typename TGameStage>
	inline bool IsCurrentStage() const;

	// Return True If The Current Stage Equal The Stage Of Specified Name
	UFUNCTION(BlueprintCallable)
	bool IsCurrentStage(const FString& InStageName) const;

	// Transition To Desired Stage
	template<typename TGameStage>
	bool TransitionToStage() { return TransitionToStage(MakeShared<TGameStage>()); }

	// Transition To Desired Stage
	UFUNCTION(BlueprintCallable)
	bool TransitionToStage(const FString& DesiredStageName);

private:
	TSharedPtr<IGameStage> CurrentStage;
};

template<typename TGameStage>
inline bool UPFGameInstance::IsCurrentStage() const
{
	return IGameStage::IsSameStage<TGameStage>(CurrentStage);
}
