// Fill out your copyright notice in the Description page of Project Settings.


#include "PFBlueprintFunctionLibrary.h"

#include "DrawDebugHelpers.h"
#include "NavigationSystem.h"
#include "PFUtils.h"

const UPFGameSettings* UPFBlueprintFunctionLibrary::GetPFGameSettings()
{
	return GetDefault<UPFGameSettings>();
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

FVector UPFBlueprintFunctionLibrary::GetRandomReachablePointOfActor(
	AActor* Actor,
	float PointAcceptedRadius,
	float AdditionalRadius,
	int32 Attempts
)
{
	if (Actor == nullptr)
	{
		return FVector::ZeroVector;
	}

	const UWorld* World = Actor->GetWorld();

	FVector Origin;
	FVector Extent;
	Actor->GetActorBounds(true, Origin, Extent);
	// DrawDebugBox(World, Origin, Extent, FColor::Yellow, true, 10, 0, 5);

	float MinRadius = FMath::Sqrt(Extent.X * Extent.X + Extent.Y * Extent.Y);
	float MaxRadius = MinRadius + PointAcceptedRadius;
	const FCollisionShape CollisionShape = FCollisionShape::MakeSphere(PointAcceptedRadius);

	FHitResult Hit;
	for (int32 i = 0; i < Attempts; i++)
	{
		const float Radius = FMath::RandRange(MinRadius, MaxRadius);
		const float Degree = FMath::RandRange(0.0f, FMath::DegreesToRadians(360.0f));

		FVector SpawnLocation = Origin + FVector(FMath::Cos(Degree), FMath::Sin(Degree), 0) * Radius;

		// DrawDebugSphere(World, Origin, MinRadius, 30, FColor::Red, true, 10, 0, 5);
		// DrawDebugSphere(World, Origin, MaxRadius, 30, FColor::Red, true, 10, 0, 5);

		World->SweepSingleByChannel(
			Hit,
			SpawnLocation,
			SpawnLocation,
			FQuat::Identity,
			ECC_Camera,
			CollisionShape
		);

		if (!Hit.bBlockingHit)
		{
			return SpawnLocation;
		}

		MinRadius += AdditionalRadius;
		MaxRadius += AdditionalRadius;
	}

	return Origin;
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

	if (const UMaterialInstanceDynamic* CurrentMaterial = Cast<UMaterialInstanceDynamic>(StaticMesh->GetMaterial(MaterialIndex)))
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

	ActorBounds = ActorBounds.ShiftBy(FVector(0, 0, 10));
	
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
		ECC_Camera,
		FCollisionShape::MakeBox(ActorBounds.GetExtent())
		
	);

	if (Hit.bBlockingHit)
	{
		// DEBUG_MESSAGE(TEXT("Hit [%s]"), *Hit.ToString());
		return false;
	}

	// Check if there is any ground below the building
	static float Step = 100.0f;
	for (float X = -ActorBounds.Min.X; X < ActorBounds.Max.X; X += Step)
	{
		for (float Y = -ActorBounds.Min.Y; Y < ActorBounds.Max.Y; Y += Step)
		{
			FVector Point = FVector(X, Y, ActorBounds.Min.Z);
			World->LineTraceSingleByChannel(
				Hit,
				Point,
				Point + FVector::DownVector * 10.0f,
				ECC_Visibility
			);
	
			if (!Hit.bBlockingHit)
			{
				return false;
			}
		}
	}

	return true;
}

// UUserWidget* UPFBlueprintFunctionLibrary::CreateAndAddWidgetTo(UObject* WorldContextObject, TSubclassOf<UUserWidget> WidgetClass,
// 	TArray<UUserWidget*>& WidgetArray, UPanelWidget* ParentPanelWidget)
// {
// 	UUserWidget* Widget = CreateWidget(WorldContextObject->GetWorld()->GetFirstPlayerController(), WidgetClass);
// 	WidgetArray.Add(Widget);
// 	ParentPanelWidget->AddChild(Widget);
// 	return Widget;
// }
