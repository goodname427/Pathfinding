// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Blueprint/UserWidget.h"
#include "GameStage/GameStage.h"
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
	TMap<FName, TSubclassOf<UUserWidget>> WidgetSettings;

public:
	virtual void Init() override;

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

	// Transition To Desired Stage
	template <typename TGameStage, typename... ArgTypes>
	bool TransitionToStage(ArgTypes&&... InArgs) { return TransitionToStage(MakeShared<TGameStage>(InArgs...)); }

private:
	TSharedPtr<IGameStage> CurrentStage;

public:
	void Error(const FString& ErrorMessage);
};

template <typename TGameStage>
inline bool UPFGameInstance::IsCurrentStage() const
{
	return IGameStage::IsSameStage<TGameStage>(CurrentStage);
}
