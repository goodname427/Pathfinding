// Fill out your copyright notice in the Description page of Project Settings.


#include "FixupCommandComponent.h"

#include "Kismet/GameplayStatics.h"

FName UFixupCommandComponent::StaticCommandName = FName("Fixup");

UFixupCommandComponent::UFixupCommandComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	Data.Name = StaticCommandName;
	
	PointPerFixup = 1;
}

bool UFixupCommandComponent::CanFixup(const ABuildingPawn* Building) const
{
	if (Building == nullptr || Building->IsPendingKill())
	{
		return false;
	}

	return GetExecutePawn()->GetPawnRole(Building) == EPawnRole::Self;
}

bool UFixupCommandComponent::InternalIsCommandEnable_Implementation() const
{
	return GetExecutePlayerState() != nullptr;
}

bool UFixupCommandComponent::InternalIsArgumentsValid_Implementation() const
{
	if (const ABuildingPawn* Building = Request.GetTargetPawn<ABuildingPawn>())
	{
		if (CanFixup(Building))
		{
			return Building->IsInBuilding() || GetExecutePlayerState()->IsResourceEnough(ResourceCostPerFixup);
		}
	}

	return false;
}

void UFixupCommandComponent::InternalBeginExecute_Implementation()
{
	Super::InternalBeginExecute_Implementation();

	ABuildingPawn* Building = Request.GetTargetPawn<ABuildingPawn>();
	const AConsciousPawn* ExecutePawn = GetExecutePawn();

	if (Building)
	{
		const TSubclassOf<UDamageType> DamageType = TSubclassOf<UDamageType>(UDamageType::StaticClass());
		const FDamageEvent DamageEvent(DamageType);

		Building->TakeDamage(PointPerFixup, DamageEvent, ExecutePawn->GetController(), GetExecutePawn());
		EndExecuteDelay(ECommandExecuteResult::Success, ExecutePawn->GetAttackDuration());
	}
	else
	{
		EndExecuteDelay(ECommandExecuteResult::Failed, ExecutePawn->GetAttackDuration());
	}
}

void UFixupCommandComponent::InternalEndExecute_Implementation(ECommandExecuteResult Result)
{
	GetExecutePawn()->SetActorScale3D(FVector(1));
}

void UFixupCommandComponent::InternalExecute_Implementation(float DeltaTime)
{
	AConsciousPawn* Pawn = GetExecutePawn();
	Pawn->SetActorScale3D(FVector(Pawn->GetActorScale3D().X + DeltaTime));
}
