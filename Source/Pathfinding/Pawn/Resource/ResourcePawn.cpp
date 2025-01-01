// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourcePawn.h"

#include "Net/UnrealNetwork.h"


AResourcePawn::AResourcePawn()
{
	PrimaryActorTick.bCanEverTick = false;

	ResourcePoint = 1;
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

bool AResourcePawn::CollectBy(ACollectorPawn* CollectorPawn)
{
	if (ResourceData.ResourceType == EResourceType::None || ResourcePoint <= 0)
	{
		return false;
	}
	
	if (CollectorPawn->CollectedResource.Type != ResourceData.ResourceType)
	{
		CollectorPawn->CollectedResource.Point = 0;
	}

	CollectorPawn->CollectedResource.Type = ResourceData.ResourceType;
	CollectorPawn->CollectedResource.Point += 1;

	ResourcePoint -= 1;
	if (ResourcePoint <= 0)
	{
		Destroy();
	}

	return true;
}
