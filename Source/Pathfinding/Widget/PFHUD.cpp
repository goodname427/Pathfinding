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

	DrawInfo();
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
	FVector2D MousePosition;

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
		

		APlayerController* PC = GetOwningPlayerController();
		if (PC)
		{
			APFPlayerState* PS = Cast<APFPlayerState>(PC->PlayerState);
			if (PS)
			{
				PlayerLocation = PS->GetPlayerLocation();
			}

			PC->GetMousePosition(MousePosition.X, MousePosition.Y);
		}
	}


	FString Text = FString::Printf(
		TEXT("Current Stage: %s\n")
		TEXT("Current Widget: %s\n")
		TEXT("World Name: %s\n")
		// TEXT("Net Mode: %s\n")
		// TEXT("Is Server: %s\n")
		// TEXT("Num Players: %d\n")
		// TEXT("Player Location: %d\n")
		TEXT("Mouse Position: %s\n")
		,
		*CurrentStageName,
		*CurrentWidgetName,
		*WorldName,
		// *NetMode,
		// *IsServer,
		// NumPlayers,
		// PlayerLocation,
		*MousePosition.ToString()
	);
	FCanvasTextItem TextItem(FVector2D(0, 0), FText::FromString(Text), GEngine->GetLargeFont(), FLinearColor::Green);
	Canvas->DrawItem(TextItem);
}


