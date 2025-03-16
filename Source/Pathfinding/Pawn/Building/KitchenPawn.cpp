// Fill out your copyright notice in the Description page of Project Settings.


#include "KitchenPawn.h"


// Sets default values
AKitchenPawn::AKitchenPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	ProducedFoodPerSecond = 1;
}

void AKitchenPawn::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		if (const UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(
				ProducedFoodTimerHandle,
				FTimerDelegate::CreateUObject(this, &AKitchenPawn::ProduceFood),
				1.f,
				true
			);
		}
	}
}

void AKitchenPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ProducedFoodTimerHandle);
	}
}

void AKitchenPawn::ProduceFood()
{
	// VALID_CHECK(this);

	if (ABattlePlayerState* PS = GetOwnerPlayer())
	{
		// VALID_CHECK(PS);
		PS->TakeResource(this, EResourceTookReason::Produce, FResourceInfo(EResourceType::Food, ProducedFoodPerSecond));
	}
}
