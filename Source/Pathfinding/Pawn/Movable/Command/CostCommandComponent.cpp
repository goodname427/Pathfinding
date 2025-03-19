// Fill out your copyright notice in the Description page of Project Settings.


#include "CostCommandComponent.h"

#include "ConsciousPawn.h"

FName UCostCommandComponent::StaticCommandName = FName("Cost");

// Sets default values for this component's properties
UCostCommandComponent::UCostCommandComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	Data.Name = StaticCommandName;

	ProgressDuration = 10.0f;
	CostCountPerCycle = 1.0f;
	HungerDamage = 10.0f;
}

void UCostCommandComponent::InternalEndExecute_Implementation(ECommandExecuteResult Result)
{
	if (ABattlePlayerState* PS = GetExecutePlayerState())
	{
		// VALID_CHECK(PS);

		const FResourceInfo FoodCost(EResourceType::Food, CostCountPerCycle);
		if (PS->IsResourceEnough(FoodCost))
		{
			PS->TakeResource(this, EResourceTookReason::FoodCostCycle, FoodCost);
		}
		else
		{
			const UPFGameSettings* GameSettings = GetDefault<UPFGameSettings>();

			UGameplayStatics::ApplyDamage(
				GetExecutePawn(),
				GameSettings ? GameSettings->HungerDamage : 10.0f,
				GetExecuteController(),
				GetExecutePawn(),
				UDamageType::StaticClass()
			);
		}
	}
}

