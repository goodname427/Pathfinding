// Fill out your copyright notice in the Description page of Project Settings.


#include "PFHUD.h"

#include "PFGameInstance.h"
#include "Engine/Canvas.h"
#include "GameFramework/GameMode.h"

void APFHUD::DrawHUD()
{
	Super::DrawHUD();

	FString CurrentStageName = TEXT("Unknown");
	UPFGameInstance* GI = Cast<UPFGameInstance>(GetGameInstance());
	if (GI)
	{
		CurrentStageName = GI->GetCurrentStageName().ToString();
	}

	FString WorldName = TEXT("Unknown");
	FString NetMode = TEXT("Unknown");
	FString IsServer = TEXT("Unknown");
	int32 NumPlayers = -1;
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
	}

	FString Text = FString::Printf(
		TEXT("Current Stage: %s\n")
		TEXT("World Name: %s\n")
		TEXT("Net Mode: %s\n")
		TEXT("Is Server: %s\n")
		TEXT("Num Players: %d\n")
		,
		*CurrentStageName,
		*WorldName,
		*NetMode,
		*IsServer,
		NumPlayers
	);
	FCanvasTextItem TextItem(FVector2D(0, 0), FText::FromString(Text), GEngine->GetLargeFont(), FLinearColor::Green);
	Canvas->DrawItem(TextItem);
}
