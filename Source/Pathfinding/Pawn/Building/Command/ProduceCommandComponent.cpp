// Fill out your copyright notice in the Description page of Project Settings.


#include "ProduceCommandComponent.h"

FName UProduceCommandComponent::StaticCommandName = FName("Produce");

UProduceCommandComponent::UProduceCommandComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	Data.Name = StaticCommandName;

	ProgressDuration = 1.0f;
	ProducedCountPerCycle = 2.0f;
}

void UProduceCommandComponent::InternalEndExecute_Implementation(ECommandExecuteResult Result)
{
	if (ABattlePlayerState* PS = GetExecutePlayerState())
	{
		// VALID_CHECK(PS);
		PS->TakeResource(this, EResourceTookReason::Produce, FResourceInfo(EResourceType::Food, ProducedCountPerCycle));
	}
}

