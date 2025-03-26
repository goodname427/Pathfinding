// Fill out your copyright notice in the Description page of Project Settings.


#include "PFBlueprintFunctionLibrary.h"

#include "DrawDebugHelpers.h"
#include "NavigationSystem.h"
#include "PFUtils.h"
#include "Kismet/GameplayStatics.h"

const UPFGameSettings* UPFBlueprintFunctionLibrary::GetPFGameSettings()
{
	return GetDefault<UPFGameSettings>();
}

bool UPFBlueprintFunctionLibrary::IsPlayerStateLocal(const APlayerState* PlayerState)
{
	// get the local player controller
	const APlayerController* LocalPlayerController = UGameplayStatics::GetPlayerController(PlayerState, 0);
	if (LocalPlayerController == nullptr)
	{
		return false;
	}

	// get the player state of the local player controller
	const APlayerState* LocalPlayerState = LocalPlayerController->PlayerState;
	if (LocalPlayerState == nullptr)
	{
		return false;
	}

	// compare the PlayerState with LocalPlayerState
	return PlayerState == LocalPlayerState;
}

APlayerController* UPFBlueprintFunctionLibrary::GetFirstLocalPlayerController(UObject* WorldContextObject)
{
	if (const UWorld* World = WorldContextObject->GetWorld())
	{
		return World->GetFirstLocalPlayerFromController()->GetPlayerController(World);
	}
	return nullptr;
}

ACommanderPawn* UPFBlueprintFunctionLibrary::GetCommanderPawn(UObject* WorldContextObject)
{
	if (const UWorld* World = WorldContextObject->GetWorld())
	{
		APlayerController* PC = World->GetFirstPlayerController();
		return GetCommanderPawnByController(PC);
	}

	return nullptr;
}

ACommanderPawn* UPFBlueprintFunctionLibrary::GetCommanderPawnByController(AController* Controller)
{
	// DEBUG_MESSAGE(TEXT("Controller = %s, Pawn = %s"), *Controller->GetName(), *(Controller->GetPawn() ? Controller->GetPawn()->GetName(): "NULL"));
	return Controller->GetPawn<ACommanderPawn>();
}

void UPFBlueprintFunctionLibrary::SendRequestTo(const FTargetRequest& TargetRequest,
                                                AConsciousPawn* ReceivedConsciousPawn)
{
	if (ACommanderPawn* CommanderPawn = GetCommanderPawn(ReceivedConsciousPawn))
	{
		CommanderPawn->SendTo(TargetRequest, ReceivedConsciousPawn);
	}
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

	FActorSpawnParameters SpawnParameters;
	{
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	}
	// DEBUG_MESSAGE(TEXT("Spawn PFPawn [%s] at Location [(%s)] for Commander [%s]"), *PFPawnClass->GetName(), *Location.ToString(), *OwnerCommander->GetPlayerState()->GetPlayerName());
	APFPawn* PFPawn = Cast<APFPawn>(World->SpawnActor(PFPawnClass, &Location, &Rotation, SpawnParameters));
	if (PFPawn)
	{
		PFPawn->SetOwner(OwnerCommander);
	}

	return PFPawn;
}

bool UPFBlueprintFunctionLibrary::GetRandomReachablePointOfPawn(
	APFPawn* Pawn,
	FVector& OutLocation,
	float PointAcceptedRadius,
	float AdditionalRadius,
	int32 Attempts
)
{
	NULL_CHECK_RET(Pawn, false);

	const UWorld* World = Pawn->GetWorld();
	NULL_CHECK_RET(World, false);

	const FVector Origin = Pawn->GetActorLocation();
	float MinRadius = Pawn->GetApproximateRadius();
	float MaxRadius = MinRadius + PointAcceptedRadius;

	const FCollisionShape CollisionShape = FCollisionShape::MakeSphere(PointAcceptedRadius);
	FHitResult Hit;

	for (int32 i = 0; i < Attempts; i++)
	{
		// if (!UNavigationSystemV1::K2_GetRandomReachablePointInRadius(Pawn, Origin, OutLocation, MinRadius))
		// {
		// 	continue;
		// }

		const float RRadius = FMath::RandRange(MinRadius, MaxRadius);
		const float RDegree = FMath::RandRange(0.0f, FMath::DegreesToRadians(360.0f));

		OutLocation = Origin + FVector(FMath::Cos(RDegree), FMath::Sin(RDegree), 0) * RRadius;
		// return true;
		// DrawDebugSphere(World, Origin, MinRadius, 30, FColor::Red, true, 10, 0, 5);
		// DrawDebugSphere(World, Origin, MaxRadius, 30, FColor::Red, true, 10, 0, 5);

		// World->SweepSingleByChannel(
		// 	Hit,
		// 	OutLocation,
		// 	OutLocation,
		// 	FQuat::Identity,
		// 	ECC_Pawn,
		// 	CollisionShape
		// );

		const FVector TestLocation = OutLocation + FVector(0, 0, PointAcceptedRadius);

		World->SweepSingleByChannel(
			Hit,
			TestLocation,
			TestLocation,
			FQuat::Identity,
			ECC_GameTraceChannel1,
			CollisionShape
		);

		if (!Hit.bBlockingHit)
		{
			return true;
		}

		MinRadius += AdditionalRadius;
		MaxRadius += AdditionalRadius;
	}

	return false;
}

void UPFBlueprintFunctionLibrary::TryCreateDynamicMaterialInstanceForStaticMesh(UStaticMeshComponent* StaticMesh,
	UMaterialInterface* Parent, int32 MaterialIndex)
{
	if (StaticMesh == nullptr || Parent == nullptr)
	{
		return;
	}

	if (MaterialIndex < 0 || MaterialIndex >= StaticMesh->GetNumMaterials())
	{
		return;
	}

	if (const UMaterialInstanceDynamic* CurrentMaterial = Cast<UMaterialInstanceDynamic>(
		StaticMesh->GetMaterial(MaterialIndex)))
	{
		if (CurrentMaterial->Parent == Parent)
		{
			return;
		}
	}

	StaticMesh->SetMaterial(MaterialIndex, UMaterialInstanceDynamic::Create(Parent, StaticMesh));
}

void UPFBlueprintFunctionLibrary::CreateDynamicMaterialInstanceForStaticMesh(
	UStaticMeshComponent* StaticMesh, UMaterialInterface* Parent, int32 MaterialIndex)
{
	if (StaticMesh == nullptr || Parent == nullptr)
	{
		return;
	}

	if (MaterialIndex < 0 || MaterialIndex >= StaticMesh->GetNumMaterials())
	{
		return;
	}

	// DEBUG_MESSAGE(TEXT("Create Dynamic Material Instance for Static Mesh [%s]"), *StaticMesh->GetName());
	StaticMesh->SetMaterial(MaterialIndex, UMaterialInstanceDynamic::Create(Parent, StaticMesh));
}

void UPFBlueprintFunctionLibrary::SetStaticMeshColor(UStaticMeshComponent* StaticMesh, FLinearColor Color,
                                                     int32 MaterialIndex)
{
	UMaterialInstanceDynamic* Material = Cast<UMaterialInstanceDynamic>(StaticMesh->GetMaterial(MaterialIndex));
	if (Material)
	{
		// DEBUG_MESSAGE(TEXT("Set Static Mesh Color [%s]"), *Color.ToString());
		Material->SetVectorParameterValue("Color", Color);
	}
}

bool UPFBlueprintFunctionLibrary::IsLocationEmptyAndOnGround(const UObject* WorldContextObject, FBox ActorBounds)
{
	const UWorld* World = WorldContextObject->GetWorld();
	NULL_CHECK_RET(World, false);

	//DEBUG_MESSAGE(TEXT("Actor Bounds [%s]"), *ActorBounds.ToString());

	ActorBounds = ActorBounds.ShiftBy(FVector(0, 0, 100));

	DrawDebugBox(
		World,
		ActorBounds.GetCenter(),
		ActorBounds.GetExtent(),
		FColor::Yellow,
		false,
		0.01f,
		0,
		5
	);

	FCollisionQueryParams Params;


	// Check if there is any blocking object at the location
	FHitResult Hit;
	World->SweepSingleByChannel(
		Hit,
		ActorBounds.GetCenter(),
		ActorBounds.GetCenter(),
		FQuat::Identity,
		ECC_GameTraceChannel1,
		FCollisionShape::MakeBox(ActorBounds.GetExtent())
	);

	if (Hit.bBlockingHit)
	{
		// DEBUG_MESSAGE(TEXT("Hit [%s]"), *Hit.ToString());
		return false;
	}

	// Check if there is any ground below the building
	// static float Step = 100.0f;
	// for (float X = -ActorBounds.Min.X; X < ActorBounds.Max.X; X += Step)
	// {
	// 	for (float Y = -ActorBounds.Min.Y; Y < ActorBounds.Max.Y; Y += Step)
	// 	{
	// 		FVector Point = FVector(X, Y, ActorBounds.Min.Z);
	// 		World->LineTraceSingleByChannel(
	// 			Hit,
	// 			Point,
	// 			Point + FVector::DownVector * 10.0f,
	// 			ECC_Visibility
	// 		);
	//
	// 		if (!Hit.bBlockingHit)
	// 		{
	// 			return false;
	// 		}
	// 	}
	// }

	return true;
}

FBox UPFBlueprintFunctionLibrary::GetCDOActorBounds(const UObject* WorldContextObject, TSubclassOf<AActor> ActorClass,
                                                    bool bNonColliding)
{
	static const FBox ForceInitBox(ForceInit);
	NULL_CHECK_RET(WorldContextObject, ForceInitBox);
	NULL_CHECK_RET(ActorClass.Get(), ForceInitBox);
	NULL_CHECK_RET(GEngine, ForceInitBox);

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	NULL_CHECK_RET(World, ForceInitBox);

	// create a temporary actor
	static FVector Location = FVector::ZeroVector;
	static FRotator Rotation = FRotator::ZeroRotator;
	static FActorSpawnParameters SpawnParams;
	{
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	}
	AActor* TempActor = World->SpawnActor(ActorClass, &Location, &Rotation, SpawnParams);
	NULL_CHECK_RET(TempActor, ForceInitBox);

	// initialize the actor
	// TempActor->PreInitializeComponents();
	// TempActor->PostInitializeComponents();
	// TempActor->RerunConstructionScripts();

	// get bounds
	const FBox ActorBounds = TempActor->GetComponentsBoundingBox(bNonColliding);

	// destroy the temporary actor
	TempActor->Destroy();

	return ActorBounds;
}

void UPFBlueprintFunctionLibrary::GetAroundPawnHitResultsAtLocation(const APFPawn* Pawn, const FVector Location,
	TArray<FHitResult>& OutHitResults, float RequiredRadius)
{
	NULL_CHECK(Pawn);
	NULL_CHECK(GEngine);

	const UWorld* World = GEngine->GetWorldFromContextObject(Pawn, EGetWorldErrorMode::LogAndReturnNull);
	NULL_CHECK(World);
	
	World->SweepMultiByChannel(
		OutHitResults,
		Location,
		Location,
		FQuat::Identity,
		ECC_GameTraceChannel1,
		FCollisionShape::MakeSphere(RequiredRadius),
		FCollisionQueryParams::DefaultQueryParam
	);
}

void UPFBlueprintFunctionLibrary::GetAroundPawnHitResults(const APFPawn* Pawn, TArray<FHitResult>& OutHitResults,
                                                          float RequiredRadius)
{
	return GetAroundPawnHitResultsAtLocation(Pawn, Pawn->GetActorLocation(), OutHitResults, RequiredRadius);
}

// UUserWidget* UPFBlueprintFunctionLibrary::CreateAndAddWidgetTo(UObject* WorldContextObject, TSubclassOf<UUserWidget> WidgetClass,
// 	TArray<UUserWidget*>& WidgetArray, UPanelWidget* ParentPanelWidget)
// {
// 	UUserWidget* Widget = CreateWidget(WorldContextObject->GetWorld()->GetFirstPlayerController(), WidgetClass);
// 	WidgetArray.Add(Widget);
// 	ParentPanelWidget->AddChild(Widget);
// 	return Widget;
// }
