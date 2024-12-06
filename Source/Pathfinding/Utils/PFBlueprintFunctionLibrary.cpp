// Fill out your copyright notice in the Description page of Project Settings.


#include "PFBlueprintFunctionLibrary.h"

#include "Components/PanelWidget.h"

const UPFGameSettings* UPFBlueprintFunctionLibrary::GetPFGameSettings()
{
	return GetDefault<UPFGameSettings>();
}

// UUserWidget* UPFBlueprintFunctionLibrary::CreateAndAddWidgetTo(UObject* WorldContextObject, TSubclassOf<UUserWidget> WidgetClass,
// 	TArray<UUserWidget*>& WidgetArray, UPanelWidget* ParentPanelWidget)
// {
// 	UUserWidget* Widget = CreateWidget(WorldContextObject->GetWorld()->GetFirstPlayerController(), WidgetClass);
// 	WidgetArray.Add(Widget);
// 	ParentPanelWidget->AddChild(Widget);
// 	return Widget;
// }
