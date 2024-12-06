// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PFPlayerState.generated.h"

class APFPlayerController;
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
	UFUNCTION(BlueprintCallable)
	APFPlayerController* GetPlayerController();

protected:
	UPROPERTY(Transient)
	APFPlayerController* PlayerController;

public:
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SetPlayerLocation(int32 InLocation);
	
public:
	UPROPERTY(Transient, EditAnywhere, BlueprintReadOnly, Replicated)
	int32 PlayerLocation;
	
	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, Replicated)
	int32 TeamId;

	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, Replicated)
	FLinearColor PlayerColor;
};
