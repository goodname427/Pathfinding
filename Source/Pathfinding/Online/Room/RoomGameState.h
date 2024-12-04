// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFGameSettings.h"
#include "PFGameState.h"
#include "RoomGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerStateChangedSignature, APlayerState*, NewPlayer);

/**
 * 
 */
UCLASS()
class PATHFINDING_API ARoomGameState : public APFGameState
{
	GENERATED_BODY()
	
	friend class ARoomGameMode;

public:
	UPROPERTY(BlueprintAssignable)
	FPlayerStateChangedSignature OnPlayerStateAdded;
	UPROPERTY(BlueprintAssignable)
	FPlayerStateChangedSignature OnPlayerStateRemoved;
	
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
public:
	UFUNCTION(BlueprintCallable)
	void SetCurrentMap(const FMapInfo& NewMap);
	
	UFUNCTION(BlueprintCallable)
	FMapInfo GetCurrentMap() const { return CurrentMap; }
	
private:
	UPROPERTY(Replicated)
	FMapInfo CurrentMap;
};
