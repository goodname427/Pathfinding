// Fill out your copyright notice in the Description page of Project Settings.


#include "PFBlueprintFunctionLibrary.h"

#include "PFUtils.h"

const UPFGameSettings* UPFBlueprintFunctionLibrary::GetPFGameSettings()
{
	return GetDefault<UPFGameSettings>();
}

ACommanderPawn* UPFBlueprintFunctionLibrary::GetCommanderPawn(UObject* WorldContextObject)
{
	APlayerController* PC = WorldContextObject->GetWorld()->GetFirstPlayerController();
	return GetCommanderPawnByController(PC);
}

ACommanderPawn* UPFBlueprintFunctionLibrary::GetCommanderPawnByController(AController* Controller)
{
	return Controller->GetPawn<ACommanderPawn>();
}

APFPawn* UPFBlueprintFunctionLibrary::SpawnPawnForCommander(
	UObject* WorldContextObject,
	TSubclassOf<APFPawn> PFPawnClass,
	ACommanderPawn* OwnerCommander,
	FVector Location,
	FRotator Rotation
)
{
	UWorld* World = WorldContextObject->GetWorld();

	// DEBUG_MESSAGE(TEXT("Spawn PFPawn [%s] at Location [(%s)] for Commander [%s]"), *PFPawnClass->GetName(), *Location.ToString(), *OwnerCommander->GetPlayerState()->GetPlayerName());
	APFPawn* PFPawn = Cast<APFPawn>(World->SpawnActor(PFPawnClass, &Location, &Rotation));
	if (PFPawn)
	{
		PFPawn->SetOwner(OwnerCommander);
	}

	return PFPawn;
}

void UPFBlueprintFunctionLibrary::CreateDynamicMaterialInstanceForStaticMesh(UStaticMeshComponent* StaticMesh, UMaterialInterface* Parent, int32 MaterialIndex)
{
	 StaticMesh->SetMaterial(MaterialIndex, UMaterialInstanceDynamic::Create(Parent, StaticMesh));
}

void UPFBlueprintFunctionLibrary::SetStaticMeshColor(UStaticMeshComponent* StaticMesh, FLinearColor Color)
{
	UMaterialInstanceDynamic* Material = Cast<UMaterialInstanceDynamic>(StaticMesh->GetMaterial(0));
	if (Material)
	{
		// DEBUG_MESSAGE(TEXT("Set Static Mesh Color [%s]"), *Color.ToString());
		Material->SetVectorParameterValue("Color", Color);
	}
}

// UUserWidget* UPFBlueprintFunctionLibrary::CreateAndAddWidgetTo(UObject* WorldContextObject, TSubclassOf<UUserWidget> WidgetClass,
// 	TArray<UUserWidget*>& WidgetArray, UPanelWidget* ParentPanelWidget)
// {
// 	UUserWidget* Widget = CreateWidget(WorldContextObject->GetWorld()->GetFirstPlayerController(), WidgetClass);
// 	WidgetArray.Add(Widget);
// 	ParentPanelWidget->AddChild(Widget);
// 	return Widget;
// }
