// Fill out your copyright notice in the Description page of Project Settings.


#include "PFHUD.h"

#include "PFGameInstance.h"
#include "PFPlayerState.h"
#include "WidgetSubsystem.h"
#include "Engine/Canvas.h"
#include "GameFramework/GameMode.h"

void APFHUD::DrawHUD()
{
	Super::DrawHUD();

	// DrawInfo();

	// draw select box
	if (bDrawingSelectBox)
	{
		DrawSelectBox();
	}
}

void APFHUD::DrawInfo() const
{
	FString CurrentStageName = TEXT("Unknown");
	FString CurrentWidgetName = TEXT("Unknown");
	UPFGameInstance* GI = Cast<UPFGameInstance>(GetGameInstance());
	if (GI)
	{
		CurrentStageName = GI->GetCurrentStageName().ToString();

		UWidgetSubsystem* WidgetSubsystem = GI->GetSubsystem<UWidgetSubsystem>();
		if (WidgetSubsystem)
		{
			CurrentWidgetName = WidgetSubsystem->GetCurrentWidgetName();
		}
	}

	FString WorldName = TEXT("Unknown");
	FString NetMode = TEXT("Unknown");
	FString IsServer = TEXT("Unknown");
	int32 NumPlayers = -1;
	int32 PlayerLocation = -1;

	UWorld* World = GetWorld();
	if (World)
	{
		WorldName = World->GetName();
		IsServer = World->IsServer() ? TEXT("True") : TEXT("False");

		switch (World->GetNetMode())
		{
		case NM_Standalone:
			NetMode = TEXT("Standalone");
			break;
		case NM_ListenServer:
			NetMode = TEXT("ListenServer");
			break;
		case NM_DedicatedServer:
			NetMode = TEXT("DedicatedServer");
			break;
		case NM_Client:
			NetMode = TEXT("Client");
			break;
		case NM_MAX:
			NetMode = TEXT("Max");
			break;
		default:
			break;
		}

		AGameModeBase* GameModeBase = World->GetAuthGameMode();
		if (GameModeBase)
		{
			NumPlayers = GameModeBase->GetNumPlayers();
		}
		APFPlayerState* PS = Cast<APFPlayerState>(GI->GetFirstLocalPlayerController()->PlayerState);
		if (PS)
		{
			PlayerLocation = PS->PlayerLocation;
		}
	}


	FString Text = FString::Printf(
		TEXT("Current Stage: %s\n")
		TEXT("Current Widget: %s\n")
		TEXT("World Name: %s\n")
		TEXT("Net Mode: %s\n")
		TEXT("Is Server: %s\n")
		TEXT("Num Players: %d\n")
		TEXT("Player Location: %d\n")
		,
		*CurrentStageName,
		*CurrentWidgetName,
		*WorldName,
		*NetMode,
		*IsServer,
		NumPlayers,
		PlayerLocation
	);
	FCanvasTextItem TextItem(FVector2D(0, 0), FText::FromString(Text), GEngine->GetLargeFont(), FLinearColor::Green);
	Canvas->DrawItem(TextItem);
}

void APFHUD::BeginDrawSelectBox()
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

FBox2D APFHUD::EndDrawSelectBox()
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

void APFHUD::DrawSelectBox() const
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
