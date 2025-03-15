// Fill out your copyright notice in the Description page of Project Settings.


#include "KitchenPawn.h"


// Sets default values
AKitchenPawn::AKitchenPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	ProducedFoodPerSecond = 1;
}

// Called when the game starts or when spawned
void AKitchenPawn::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		return;
	}

	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(ProducedFoodTimerHandle, FTimerDelegate::CreateLambda([this]
		{
			VALID_CHECK(this);
			
			if (ABattlePlayerState* PS = GetOwnerPlayer())
			{
				VALID_CHECK(PS);
				PS->TakeResource(this, EResourceTookReason::Produce, FResourceInfo(EResourceType::Food, ProducedFoodPerSecond));
			}
		}), 1.f, true);
	}	
}

void AKitchenPawn::OnDied()
{
	Super::OnDied();

	if (!HasAuthority())
	{
		return;
	}

	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ProducedFoodTimerHandle);
	}
}
