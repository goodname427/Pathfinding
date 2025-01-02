// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourcePawn.h"

#include "PFUtils.h"
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
	if (ResourceData.Type == EResourceType::None || ResourcePoint <= 0)
	{
		return false;
	}
	
	if (CollectorPawn->IsCollectedResourceFull())
	{
		return false;
	}

	CollectorPawn->SetCollectedResourceType(ResourceData.Type);

	const int32 ActualPoint = FMath::Min(
		CollectorPawn->GetMaxAvailableCollectedResourcePoint(),
		ResourcePoint
	);

	CollectorPawn->SetCollectedResourcePoint(CollectorPawn->GetCollectedResource().Point + ActualPoint);
	ResourcePoint -= ActualPoint;

	if (ResourcePoint <= 0)
	{
		Destroy();
	}

	return true;
}
