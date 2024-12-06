// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFGameSettings.h"
#include "PFGameState.h"
#include "PFPlayerState.h"
#include "RoomGameState.generated.h"



/**
 * 
 */
UCLASS()
class PATHFINDING_API ARoomGameState : public APFGameState
{
	GENERATED_BODY()
	
	friend class ARoomGameMode;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	UFUNCTION(BlueprintCallable)
	void SetCurrentMap(const FMapInfo& NewMap);
	
	UFUNCTION(BlueprintCallable)
	FMapInfo GetCurrentMap() const { return CurrentMap; }
	
private:
	UPROPERTY(Transient, Replicated)
	FMapInfo CurrentMap;
};
