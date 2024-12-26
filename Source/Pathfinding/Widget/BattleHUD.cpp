// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleHUD.h"

#include "CanvasItem.h"
#include "CommanderPawn.h"
#include "Engine/Canvas.h"

void ABattleHUD::BeginPlay()
{
	Super::BeginPlay();
	OwningCommanderPawn = Cast<ACommanderPawn>(GetOwningPawn());
}

void ABattleHUD::DrawHUD()
{
	Super::DrawHUD();

	if (OwningCommanderPawn && OwningCommanderPawn->IsSelecting())
	{
		if (!bSelectBoxBeginMousePosHasSet)
		{
			APlayerController* PlayerController = GetOwningPlayerController();
			if (PlayerController)
			{
				bSelectBoxBeginMousePosHasSet = true;
				PlayerController->GetMousePosition(SelectBoxBeginMousePos.X, SelectBoxBeginMousePos.Y);
			}
		}

		DrawSelectBox();
	}
	else if (bSelectBoxBeginMousePosHasSet)
	{
		bSelectBoxBeginMousePosHasSet = false;
	}

	if (OwningCommanderPawn && OwningCommanderPawn->IsTargeting())
	{
		DrawTargetingHint();
	}
}

void ABattleHUD::DrawSelectBox() const
{
	const APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController)
	{
		FVector2D MousePos;
		PlayerController->GetMousePosition(MousePos.X, MousePos.Y);

		FCanvasBoxItem BoxItem(SelectBoxBeginMousePos, MousePos - SelectBoxBeginMousePos);
		Canvas->DrawItem(BoxItem);
	}
}

void ABattleHUD::DrawTargetingHint() const
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController)
	{
		FVector2D MousePos;
		PlayerController->GetMousePosition(MousePos.X, MousePos.Y);

		FCanvasTextItem BoxItem(MousePos + FVector2D(10, 10), FText::FromString(OwningCommanderPawn->GetTargetingCommandName().ToString()),
		                        GEngine->GetLargeFont(), FColor::Red);
		Canvas->DrawItem(BoxItem);
	}
}
