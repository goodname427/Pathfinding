// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PFPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class PATHFINDING_API APFPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	int32 TeamId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	FLinearColor PlayerColor;
};
