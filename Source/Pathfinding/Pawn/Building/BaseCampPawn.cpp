// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCampPawn.h"

#include "PFUtils.h"


// Sets default values
ABaseCampPawn::ABaseCampPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void ABaseCampPawn::TransportBy(ACollectorPawn* CollectorPawn)
{
	if (CollectorPawn->CollectedResource == 0 || CollectorPawn->CollectedResourceType == EResourceType::None)
	{
		return;
	}
	
	ABattlePlayerState* PS = GetOwnerPlayer();
	
	PS->SetResource(CollectorPawn->CollectedResourceType,
	                PS->GetResource(CollectorPawn->CollectedResourceType) + CollectorPawn->
	                CollectedResource);

	CollectorPawn->CollectedResource = 0;
	CollectorPawn->CollectedResourceType = EResourceType::None;
}
