// Fill out your copyright notice in the Description page of Project Settings.


#include "CollectorComponent.h"


// Sets default values for this component's properties
UCollectorComponent::UCollectorComponent(): ResourcePointPerCollecting(2), MaxCollectedResourcePoint(10)
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UCollectorComponent::SetCollectedResourceType(EResourceType NewResourceType)
{
	if (NewResourceType != CollectedResource.Type)
	{
		CollectedResource.Point = 0;
	}

	CollectedResource.Type = NewResourceType;
}

