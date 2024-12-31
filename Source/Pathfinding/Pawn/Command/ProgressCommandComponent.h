// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommandComponent.h"
#include "ProgressCommandComponent.generated.h"


UCLASS(Abstract, ClassGroup=(Command), meta=(BlueprintSpawnableComponent))
class PATHFINDING_API UProgressCommandComponent : public UCommandComponent
{
	GENERATED_BODY()

public:
	UProgressCommandComponent();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	DECLARE_COMMAND_CHANNEL()
	
	UFUNCTION(BlueprintCallable)
	float GetNormalizedProgress() const { return 1.0f - RemainedProgress / GetProgressDuration(); }	

	UFUNCTION(BlueprintNativeEvent)
	float GetProgressDuration() const;

protected:
	virtual bool InternalIsReachable_Implementation() override;
	
protected:
	UPROPERTY(Category = "Command|Progress", EditAnywhere)
	float ProgressDuration;

	UPROPERTY(Category = "Command|Progress", EditAnywhere, meta=(AllowedClasses="Texture"))
	UObject* ProgressIcon;

	float RemainedProgress;
	
protected:
	virtual void InternalBeginExecute_Implementation() override;
};
