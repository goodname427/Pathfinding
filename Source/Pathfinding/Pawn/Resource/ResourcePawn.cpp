// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourcePawn.h"

#include "PFUtils.h"
#include "Movable/Component/CollectorComponent.h"
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

void AResourcePawn::PostInitProperties()
{
	MaxHealth = ResourcePoint;
	
	Super::PostInitProperties();
}

bool AResourcePawn::CollectBy(UCollectorComponent* Collector)
{
	if (ResourceData.Type == EResourceType::None || ResourcePoint <= 0)
	{
		return false;
	}
	
	if (Collector->IsCollectedResourceFull())
	{
		return false;
	}

	Collector->SetCollectedResourceType(ResourceData.Type);

	const int32 ActualPoint = FMath::Min(
		Collector->GetMaxAvailableCollectedResourcePoint(),
		ResourcePoint
	);

	Collector->SetCollectedResourcePoint(Collector->GetCollectedResource().Point + ActualPoint);
	ResourcePoint -= ActualPoint;

	CurrentHealth = ResourcePoint;

	if (ResourcePoint <= 0)
	{
		Die();
	}

	return true;
}
