// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourcePawn.h"


AResourcePawn::AResourcePawn(): ResourcePoint(0), ResourceType(EResourceType::Coin)
{
	PrimaryActorTick.bCanEverTick = false;

	MaxHealth = 0;
	Attack = 0;
	Defense = 0;
	AttackSpeed = 0;
}

void AResourcePawn::CollectBy(ACollectorPawn* CollectorPawn)
{
	if (ResourceType == EResourceType::None || ResourcePoint <= 0)
	{
		return;
	}
	
	if (CollectorPawn->CollectedResourceType != ResourceType)
	{
		CollectorPawn->CollectedResource = 0;
	}

	CollectorPawn->CollectedResourceType = ResourceType;
	CollectorPawn->CollectedResource += 1;

	ResourcePoint -= 1;
	if (ResourcePoint <= 0)
	{
		Destroy();
	}
}
