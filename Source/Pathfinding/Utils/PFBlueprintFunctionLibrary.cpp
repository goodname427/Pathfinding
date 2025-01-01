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

	// DEBUG_MESSAGE(TEXT("Spawn PFPawn [%s] at Location [(%s)] for Commander [%s]"), *PFPawnClass->GetName(), *Location.ToString(), *OwnerCommander->GetPlayerState()->GetPlayerName());
	APFPawn* PFPawn = Cast<APFPawn>(World->SpawnActor(PFPawnClass, &Location, &Rotation));
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

void UPFBlueprintFunctionLibrary::CreateDynamicMaterialInstanceForStaticMesh(
	UStaticMeshComponent* StaticMesh, UMaterialInterface* Parent, int32 MaterialIndex)
{
	if (StaticMesh == nullptr || Parent == nullptr)
	{
		return;
	}

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

// UUserWidget* UPFBlueprintFunctionLibrary::CreateAndAddWidgetTo(UObject* WorldContextObject, TSubclassOf<UUserWidget> WidgetClass,
// 	TArray<UUserWidget*>& WidgetArray, UPanelWidget* ParentPanelWidget)
// {
// 	UUserWidget* Widget = CreateWidget(WorldContextObject->GetWorld()->GetFirstPlayerController(), WidgetClass);
// 	WidgetArray.Add(Widget);
// 	ParentPanelWidget->AddChild(Widget);
// 	return Widget;
// }
