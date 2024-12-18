// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "PFPlayerState.h"
#include "PFGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerLocationChangedSignature);

/**
 * 
 */
UCLASS()
class PATHFINDING_API APFGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	void InitPlayerLocations(int32 MaxPlayer);

	void InitPlayerLocations();
	
	UFUNCTION(BlueprintCallable)
	void SetPlayerLocation(int32 InLocation, APFPlayerState* Player);

	UFUNCTION(BlueprintCallable)
	const APFPlayerState* GetPlayerLocation(int32 Location) const { return PlayerLocations[Location]; }

	UFUNCTION(BlueprintCallable)
	int32 GetNumPlayerLocations() const { return PlayerLocations.Num(); }
	
	UPROPERTY(BlueprintAssignable)
	FPlayerLocationChangedSignature OnPlayerLocationChanged;

private:
	UPROPERTY(Transient, ReplicatedUsing = OnRep_PlayerLocations)
	TArray<APFPlayerState*> PlayerLocations;
	UFUNCTION()
	void OnRep_PlayerLocations();
};
