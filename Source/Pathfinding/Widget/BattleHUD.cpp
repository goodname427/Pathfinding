// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleHUD.h"

#include "CanvasItem.h"
#include "Engine/Canvas.h"

void ABattleHUD::DrawHUD()
{
	Super::DrawHUD();

	if (bDrawingSelectBox)
	{
		DrawSelectBox();
	}
}

void ABattleHUD::BeginDrawSelectBox()
{
	if (bDrawingSelectBox)
	{
		EndDrawSelectBox();
	}
	
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController)
	{
		bDrawingSelectBox = true;
		PlayerController->GetMousePosition(SelectBoxBeginMousePos.X, SelectBoxBeginMousePos.Y);
	}
}

FBox2D ABattleHUD::EndDrawSelectBox()
{
	if (!bDrawingSelectBox)
	{
		return FBox2D();
	}

	FVector2D MousePos;
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController)
	{
		PlayerController->GetMousePosition(MousePos.X, MousePos.Y);
	}
	
	bDrawingSelectBox = false;
	return FBox2D(FVector2D::Min(SelectBoxBeginMousePos, MousePos), FVector2D::Max(SelectBoxBeginMousePos, MousePos));
}

void ABattleHUD::DrawSelectBox() const
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController)
	{
		FVector2D MousePos;
		PlayerController->GetMousePosition(MousePos.X, MousePos.Y);

		FCanvasBoxItem BoxItem(SelectBoxBeginMousePos, MousePos - SelectBoxBeginMousePos);
		Canvas->DrawItem(BoxItem);
	}
}
