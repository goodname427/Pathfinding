// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommanderPawn.h"
#include "PFHUD.h"
#include "BattleHUD.generated.h"

/**
 * 
 */
UCLASS()
class PATHFINDING_API ABattleHUD : public APFHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY()
	ACommanderPawn* OwningCommanderPawn;
	
public:
	virtual void DrawHUD() override;

protected:
	void DrawSelectBox() const;

private:
	uint32 bSelectBoxBeginMousePosHasSet : 1;
	FVector2D SelectBoxBeginMousePos;

protected:
	void DrawTargetingHint() const;
};
