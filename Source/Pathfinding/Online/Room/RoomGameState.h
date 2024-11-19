// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFGameInstance.h"
#include "PFGameState.h"
#include "RoomGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCurrentMapChangedSignature);

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
	UPROPERTY(BlueprintAssignable)
	FCurrentMapChangedSignature OnCurrentMapChanged;
	
	UFUNCTION(BlueprintCallable)
	FMapInfo GetCurrentMap() const { return CurrentMap; }
	
private:
	UFUNCTION()
	void OnRep_CurrentMap();
	UPROPERTY(ReplicatedUsing = OnRep_CurrentMap)
	FMapInfo CurrentMap;

	void SetCurrentMap(const FMapInfo& NewMap);
};
