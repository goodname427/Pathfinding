// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplodeAttackCommandComponent.h"

#include "PFBlueprintFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

FName UExplodeAttackCommandComponent::StaticCommandName = TEXT("Explode");

// Sets default values for this component's properties
UExplodeAttackCommandComponent::UExplodeAttackCommandComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	Data.Name = StaticCommandName;

	ExplodeRadius = 500;
}

bool UExplodeAttackCommandComponent::InternalIsArgumentsValid_Implementation() const
{
	return true;
}

void UExplodeAttackCommandComponent::InternalBeginExecute_Implementation()
{
	AUTHORITY_CHECK();
	
	ApplyDamageToTargetPawn();
	
	EndExecute(ECommandExecuteResult::Success);

	GetExecutePawn()->Die();
}

void UExplodeAttackCommandComponent::InternalEndExecute_Implementation(ECommandExecuteResult Result)
{
	// Skip the implementation of parent class
}

void UExplodeAttackCommandComponent::InternalExecute_Implementation(float DeltaTime)
{
	// Skip the implementation of parent class
}

void UExplodeAttackCommandComponent::ApplyDamageToTargetPawn_Implementation()
{
	AConsciousPawn* ExecutePawn = GetExecutePawn();

	static TArray<APFPawn*> AroundPawns;
	UPFBlueprintFunctionLibrary::GetAroundPawns(ExecutePawn, AroundPawns, ExplodeRadius);
	for (auto& Pawn : AroundPawns)
	{
		UGameplayStatics::ApplyDamage(
			Pawn,
			ExecutePawn->GetAttack(),
			ExecutePawn->GetController(),
			ExecutePawn,
			UDamageType::StaticClass()
		);
	}
}
