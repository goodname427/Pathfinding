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
	static bool IsPlayerStateLocal(const APlayerState* PlayerState);

	UFUNCTION(BlueprintCallable)
	static APlayerController* GetFirstLocalPlayerController(UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable)
	static ACommanderPawn* GetCommanderPawn(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable)
	static ACommanderPawn* GetCommanderPawnByController(AController* Controller);

	UFUNCTION(BlueprintCallable)
	static void SendRequestTo(const FTargetRequest& TargetRequest, AConsciousPawn* ReceivedConsciousPawn);

	UFUNCTION(BlueprintCallable)
	static APFPawn* SpawnPawnForCommander(
		UObject* WorldContextObject,
		TSubclassOf<APFPawn> PFPawnClass,
		ACommanderPawn* OwnerCommander,
		FVector Location,
		FRotator Rotation
	);

	UFUNCTION(BlueprintCallable)
	static bool GetRandomReachablePointOfPawn(APFPawn* Pawn,
	                                          FVector& OutLocation,
	                                          float PointAcceptedRadius = 100,
	                                          float AdditionalRadius = 100,
	                                          int32 Attempts = 8);
	
public:
	static void TryCreateDynamicMaterialInstanceForStaticMesh(UStaticMeshComponent* StaticMesh,
	                                                          UMaterialInterface* Parent,
	                                                          int32 MaterialIndex = 0);

	UFUNCTION(BlueprintCallable)
	static void CreateDynamicMaterialInstanceForStaticMesh(UStaticMeshComponent* StaticMesh, UMaterialInterface* Parent,
	                                                       int32 MaterialIndex = 0);

	UFUNCTION(BlueprintCallable)
	static void SetStaticMeshColor(UStaticMeshComponent* StaticMesh, FLinearColor Color, int32 MaterialIndex = 0);

public:
	UFUNCTION(BlueprintCallable)
	static bool IsLocationEmptyAndOnGround(const UObject* WorldContextObject, FBox ActorBounds);

	UFUNCTION(BlueprintCallable)
	static FBox GetCDOActorBounds(const UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, bool bNonColliding = false);
	
	template<class TPFPawn>
	static TPFPawn* GetAroundPawn(const APFPawn* Pawn, float RequiredRadius, EPawnRole RequiredPawnRole = EPawnRole::None);

	template<class TPFPawn>
	static void GetAroundPawns(const APFPawn* Pawn, const FVector Location, TArray<TPFPawn*>& OutPawns, float RequiredRadius, EPawnRole RequiredPawnRole = EPawnRole::None);
	
	template<class TPFPawn>
	static void GetAroundPawns(const APFPawn* Pawn, TArray<TPFPawn*>& OutPawns, float RequiredRadius, EPawnRole RequiredPawnRole = EPawnRole::None);

	UFUNCTION(BlueprintCallable)
	static void GetAroundPawns(const APFPawn* Pawn, TArray<APFPawn*>& OutPawns, float RequiredRadius, EPawnRole RequiredPawnRole = EPawnRole::None)
	{
		GetAroundPawns<APFPawn>(Pawn, OutPawns, RequiredRadius, RequiredPawnRole);
	}

	UFUNCTION(BlueprintCallable)
	static void GetAroundPawnHitResultsAtLocation(const APFPawn* Pawn, const FVector Location, TArray<FHitResult>& OutHitResults, float RequiredRadius);
	
	UFUNCTION(BlueprintCallable)
	static void GetAroundPawnHitResults(const APFPawn* Pawn, TArray<FHitResult>& OutHitResults, float RequiredRadius);

public:
	UFUNCTION(BlueprintCallable)
	static FString AppendNewLineIfStringNotEmpty(const FString& A, const FString& B);

	UFUNCTION(BlueprintCallable)
	static FString BreakStringByPunctuation(const FString& InString);
};

template <class TPFPawn>
TPFPawn* UPFBlueprintFunctionLibrary::GetAroundPawn(const APFPawn* Pawn, float RequiredRadius,
	EPawnRole RequiredPawnRole)
{
	static TArray<FHitResult> HitResults;
	GetAroundPawnHitResults(Pawn, HitResults, RequiredRadius);

	for (const FHitResult& Hit : HitResults)
	{
		TPFPawn* AroundPawn = Cast<TPFPawn>(Hit.GetActor());
		if (AroundPawn == nullptr || !AroundPawn->IsValidLowLevelFast())
		{
			continue;
		}

		// Skip self
		if (AroundPawn == Pawn)
		{
			continue;
		}

		// Skip if pawn role is not required
		if (RequiredPawnRole == EPawnRole::None || Pawn->GetPawnRole(AroundPawn) == RequiredPawnRole)
		{
			return AroundPawn;
		}
	}

	return nullptr;
}

template <class TPFPawn>
void UPFBlueprintFunctionLibrary::GetAroundPawns(const APFPawn* Pawn, const FVector Location,
	TArray<TPFPawn*>& OutPawns, float RequiredRadius, EPawnRole RequiredPawnRole)
{
	static TArray<FHitResult> HitResults;
	GetAroundPawnHitResultsAtLocation(Pawn, Location,HitResults, RequiredRadius);

	OutPawns.Empty();
	for (const FHitResult& Hit : HitResults)
	{
		TPFPawn* AroundPawn = Cast<TPFPawn>(Hit.GetActor());
		if (AroundPawn == nullptr || !AroundPawn->IsValidLowLevelFast())
		{
			continue;
		}

		// Skip self
		if (AroundPawn == Pawn)
		{
			continue;
		}

		// Skip if pawn role is not required
		if (RequiredPawnRole == EPawnRole::None || Pawn->GetPawnRole(AroundPawn) == RequiredPawnRole)
		{
			OutPawns.AddUnique(AroundPawn);
		}
	}
}

template <class TPFPawn>
void UPFBlueprintFunctionLibrary::GetAroundPawns(const APFPawn* Pawn, TArray<TPFPawn*>& OutPawns, float RequiredRadius,
	EPawnRole RequiredPawnRole)
{
	
}
