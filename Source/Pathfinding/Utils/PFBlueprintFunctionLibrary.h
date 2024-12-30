// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFGameSettings.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CommanderPawn.h"
#include "PFBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class PATHFINDING_API UPFBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static const UPFGameSettings* GetPFGameSettings();

	// UFUNCTION(BlueprintCallable)
	// static UUserWidget* CreateAndAddWidgetTo(UObject* WorldContextObject, TSubclassOf<UUserWidget> WidgetClass, TArray<UUserWidget*>& WidgetArray, UPanelWidget* ParentPanelWidget);

public:
	UFUNCTION(BlueprintCallable)
	static ACommanderPawn* GetCommanderPawn(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable)
	static ACommanderPawn* GetCommanderPawnByController(AController* Controller);

	UFUNCTION(BlueprintCallable)
	static APFPawn* SpawnPawnForCommander(
		UObject* WorldContextObject,
		TSubclassOf<APFPawn> PFPawnClass,
		ACommanderPawn* OwnerCommander,
		FVector Location,
		FRotator Rotation
	);

	UFUNCTION(BlueprintCallable)
	static FVector GetRandomReachablePointOfActor(AActor* Actor, float PointAcceptedRadius = 100.f, float AdditionalRadius = 100.0f, int32 Attempts = 10);

public:
	UFUNCTION(BlueprintCallable)
	static void CreateDynamicMaterialInstanceForStaticMesh(UStaticMeshComponent* StaticMesh, UMaterialInterface* Parent,
	                                                       int32 MaterialIndex = 0);

	UFUNCTION(BlueprintCallable)
	static void SetStaticMeshColor(UStaticMeshComponent* StaticMesh, FLinearColor Color, int32 MaterialIndex = 0);
};
