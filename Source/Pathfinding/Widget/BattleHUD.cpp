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

	if (OwningCommanderPawn)
	{
		DrawCommand();
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

		FCanvasTextItem TextItem(MousePos + FVector2D(10, 10),
		                         FText::FromString(OwningCommanderPawn->GetTargetingCommandName().ToString()),
		                         GEngine->GetLargeFont(), FColor::Red);
		Canvas->DrawItem(TextItem);
	}
}

void ABattleHUD::DrawCommand() const
{
	const AConsciousPawn* ConsciousPawn = Cast<AConsciousPawn>(OwningCommanderPawn->GetFirstSelectedPawn());
	APlayerController* PlayerController = GetOwningPlayerController();
	if (ConsciousPawn && PlayerController)
	{
		FVector2D MousePos;

		PlayerController->GetMousePosition(MousePos.X, MousePos.Y);

		const UCommandComponent* CurrentCommand = ConsciousPawn->GetCurrentCommand(GCommandChannel_Default);
		const TArray<UCommandComponent*>& Commands = ConsciousPawn->GetCommandsInQueue(GCommandChannel_Default);

		int32 i = 0;
		auto Draw = [&](const UCommandComponent* Command, const FLinearColor& Color)
		{
			if (Command)
			{
				FCanvasTextItem TextItem(MousePos + FVector2D(10 , 20 * i),
				                         FText::FromString(FString::Printf(TEXT("%d.%s"), i, *Command->GetCommandName().ToString())),
				                         GEngine->GetLargeFont(), Color);
				Canvas->DrawItem(TextItem);
			}

			i++;
		};

		Draw(CurrentCommand, FColor::Red);
		for (const UCommandComponent* Command : Commands)
		{
			Draw(Command,FColor::Yellow);
		}
	}
}
