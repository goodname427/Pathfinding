// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFHUD.h"
#include "BattleHUD.generated.h"

/**
 * 
 */
UCLASS()
class PATHFINDING_API ABattleHUD : public APFHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;
	
public:
	void BeginDrawSelectBox();
	FBox2D EndDrawSelectBox();

private:
	void DrawSelectBox() const;

private:
	int32 bDrawingSelectBox : 1;
	FVector2D SelectBoxBeginMousePos;
};
