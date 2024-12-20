// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourcePawn.h"

#include "Net/UnrealNetwork.h"


AResourcePawn::AResourcePawn()
{
	PrimaryActorTick.bCanEverTick = false;

	ResourcePoint = 0;
	MaxHealth = 0;
	Attack = 0;
	AttackSpeed = 0;
	Defense = 0;
}

void AResourcePawn::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ResourcePoint);
}

void AResourcePawn::CollectBy(ACollectorPawn* CollectorPawn)
{
	if (ResourceData.ResourceType == EResourceType::None || ResourcePoint <= 0)
	{
		return;
	}
	
	if (CollectorPawn->CollectedResourceType != ResourceData.ResourceType)
	{
		CollectorPawn->CollectedResource = 0;
	}

	CollectorPawn->CollectedResourceType = ResourceData.ResourceType;
	CollectorPawn->CollectedResource += 1;

	ResourcePoint -= 1;
	if (ResourcePoint <= 0)
	{
		Destroy();
	}
}
