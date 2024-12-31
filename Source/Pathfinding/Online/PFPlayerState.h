// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
// #include "PFGameState.h"
#include "PFGameSettings.h"
#include "GameFramework/PlayerState.h"
#include "PFPlayerState.generated.h"

class UCamp;
class APFPlayerController;
/**
 * 
 */
UCLASS()
class PATHFINDING_API APFPlayerState : public APlayerState
{
	GENERATED_BODY()

	friend class APFGameState;
	
public:
	APFPlayerState();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void CopyProperties(APlayerState* PlayerState) override;
	
public:
	UFUNCTION(BlueprintCallable)
	APFPlayerController* GetPlayerController();
	
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SetPlayerLocation(int32 InLocation);

	UFUNCTION(BlueprintCallable)
	int32 GetPlayerLocation() const { return PlayerLocation; }

	UFUNCTION(BlueprintCallable)
	int32 GetTeamId() const { return TeamId; }

	UFUNCTION(BlueprintCallable)
	FLinearColor GetPlayerColor() const { return PlayerColor; }
	
	UFUNCTION(BlueprintCallable)
	const UCamp* GetCamp() const { return Camp; }

	UFUNCTION(BlueprintCallable)
	void SetCamp(const UCamp* InCampInfo) { Camp = InCampInfo; }
	
protected:
	UPROPERTY(Transient, Replicated)
	int32 PlayerLocation;
	
	UPROPERTY(Transient, Replicated)
	int32 TeamId;

	UPROPERTY(Transient, Replicated)
	FLinearColor PlayerColor;

	UPROPERTY(Transient, Replicated)
	const UCamp* Camp;
};
